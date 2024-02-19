/*
 * Distributed under the OpenDDS License.
 * See: http://www.opendds.org/license.html
 */

#include "DataReaderListenerImpl.h"
#include "MessengerTypeSupportImpl.h"

#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/WaitSet.h>
#include <dds/DCPS/StaticIncludes.h>
#if OPENDDS_DO_MANUAL_STATIC_INCLUDES
#  include <dds/DCPS/RTPS/RtpsDiscovery.h>
#  include <dds/DCPS/transport/rtps_udp/RtpsUdp.h>
#endif

#include <dds/DdsDcpsInfrastructureC.h>
#include <dds/DdsDcpsSubscriptionC.h>

#include <ace/Log_Msg.h>

void child_process() {
    int sockfd;
    struct sockaddr_in addr;
    char buffer[1024];

    // 소켓 생성
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    // set reuse port
    int optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) < 0) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Enable broadcast
    int broadcastEnable=1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
        perror("Error in setting broadcast option");
        exit(EXIT_FAILURE);
    }
    memset(&addr, 0, sizeof(addr));

    // Filling server information
    addr.sin_family = AF_INET;
    addr.sin_port = htons(17900);
    addr.sin_addr.s_addr = inet_addr("239.255.0.1");

    // 바인딩
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("bind success\n");
    // 데이터 수신
    int n = recvfrom(sockfd, (char*)buffer, 1024, MSG_WAITALL, NULL, 0);
    buffer[n] = '\0';
    close(sockfd);

    // Calculate PORT
    // -------------------------
    int offset = 204 - 44;
    unsigned short port = 0;
    unsigned char tmp1 = buffer[offset];
    unsigned char tmp2 = buffer[offset + 1];
    port = (tmp2 << 8) | tmp1;

    // Create a new socket
    int recv_socket;
    if ((recv_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    // set reuse port
    if (setsockopt(recv_socket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) < 0) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Set the address and port
    struct sockaddr_in addr2;
    memset(&addr2, 0, sizeof(addr2));
    addr2.sin_family = AF_INET;
    addr2.sin_port = htons(12355);
    addr2.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(recv_socket, (struct sockaddr*)&addr2, sizeof(addr2)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    while (true) {
        char buffer[1024];
        int n = recvfrom(recv_socket, (char*)buffer, 1024, MSG_WAITALL, NULL, 0);
        buffer[n] = '\0';
        printf("Received: %s\n", buffer);
        // Proxy it to the original port
        struct sockaddr_in addr3;
        memset(&addr3, 0, sizeof(addr3));
        addr3.sin_family = AF_INET;
        addr3.sin_port = htons(port);
        addr3.sin_addr.s_addr = inet_addr("127.0.0.1");
        sendto(recv_socket, (const char*)buffer, n, MSG_CONFIRM, (const struct sockaddr*)&addr3, sizeof(addr3));

    }

    close(recv_socket);

}
int ACE_TMAIN(int argc, ACE_TCHAR* argv[])
{
  try {
    pid_t pid = fork();
    printf("pid: %d\n", pid);
    if (pid == 0) {
      child_process();
      return 0;
    }
    // Initialize DomainParticipantFactory
    DDS::DomainParticipantFactory_var dpf =
      TheParticipantFactoryWithArgs(argc, argv);

    // Create DomainParticipant
    DDS::DomainParticipant_var participant =
      dpf->create_participant(42,
                              PARTICIPANT_QOS_DEFAULT,
                              0,
                              OpenDDS::DCPS::DEFAULT_STATUS_MASK);

    if (!participant) {
      ACE_ERROR_RETURN((LM_ERROR,
                        ACE_TEXT("ERROR: %N:%l: main() -")
                        ACE_TEXT(" create_participant failed!\n")),
                       1);
    }

    // Register Type (Messenger::Message)
    HelloWorldTypeSupport_var ts =
      new HelloWorldTypeSupportImpl;

    if (ts->register_type(participant, "") != DDS::RETCODE_OK) {
      ACE_ERROR_RETURN((LM_ERROR,
                        ACE_TEXT("ERROR: %N:%l: main() -")
                        ACE_TEXT(" register_type failed!\n")),
                       1);
    }

    // Create Topic (HelloWorld)
    CORBA::String_var type_name = ts->get_type_name();
    DDS::Topic_var topic =
      participant->create_topic("HelloWorld",
                                type_name,
                                TOPIC_QOS_DEFAULT,
                                0,
                                OpenDDS::DCPS::DEFAULT_STATUS_MASK);

    if (!topic) {
      ACE_ERROR_RETURN((LM_ERROR,
                        ACE_TEXT("ERROR: %N:%l: main() -")
                        ACE_TEXT(" create_topic failed!\n")),
                       1);
    }

    // Create Subscriber
    DDS::Subscriber_var subscriber =
      participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT,
                                     0,
                                     OpenDDS::DCPS::DEFAULT_STATUS_MASK);

    if (!subscriber) {
      ACE_ERROR_RETURN((LM_ERROR,
                        ACE_TEXT("ERROR: %N:%l: main() -")
                        ACE_TEXT(" create_subscriber failed!\n")),
                       1);
    }

    // Create DataReader
    DDS::DataReaderListener_var listener(new DataReaderListenerImpl);

    DDS::DataReaderQos reader_qos;
    subscriber->get_default_datareader_qos(reader_qos);
    reader_qos.reliability.kind = DDS::RELIABLE_RELIABILITY_QOS;

    DDS::DataReader_var reader =
      subscriber->create_datareader(topic,
                                    reader_qos,
                                    listener,
                                    OpenDDS::DCPS::DEFAULT_STATUS_MASK);

    if (!reader) {
      ACE_ERROR_RETURN((LM_ERROR,
                        ACE_TEXT("ERROR: %N:%l: main() -")
                        ACE_TEXT(" create_datareader failed!\n")),
                       1);
    }

    HelloWorldDataReader_var reader_i =
      HelloWorldDataReader::_narrow(reader);

    if (!reader_i) {
      ACE_ERROR_RETURN((LM_ERROR,
                        ACE_TEXT("ERROR: %N:%l: main() -")
                        ACE_TEXT(" _narrow failed!\n")),
                       1);
    }

    // Block until Publisher completes
    printf("Subscriber is waiting for publisher\n");
    DDS::StatusCondition_var condition = reader->get_statuscondition();
    condition->set_enabled_statuses(DDS::SUBSCRIPTION_MATCHED_STATUS);
    printf("Subscriber is successfully connecting publisher\n");
    DDS::WaitSet_var ws = new DDS::WaitSet;
    ws->attach_condition(condition);
    std::cout << "Waiting for publisher" << std::endl;
    while (true) {  
      DDS::SubscriptionMatchedStatus matches;
      if (reader->get_subscription_matched_status(matches) != DDS::RETCODE_OK) {
        ACE_ERROR_RETURN((LM_ERROR,
                          ACE_TEXT("ERROR: %N:%l: main() -")
                          ACE_TEXT(" get_subscription_matched_status failed!\n")),
                         1);
      }

      if (matches.current_count == 0 && matches.total_count > 0) {
        break;
      }

      DDS::ConditionSeq conditions;
      DDS::Duration_t timeout = { 60, 0 };
      printf("Subscriber is waiting for publisher\n");
      if (ws->wait(conditions, timeout) != DDS::RETCODE_OK) {
        ACE_ERROR_RETURN((LM_ERROR,
                          ACE_TEXT("ERROR: %N:%l: main() -")
                          ACE_TEXT(" wait failed!\n")),
                         1);
      }
    }

    ws->detach_condition(condition);

    // Clean-up!
    participant->delete_contained_entities();
    dpf->delete_participant(participant);

    TheServiceParticipant->shutdown();

  } catch (const CORBA::Exception& e) {
    e._tao_print_exception("Exception caught in main():");
    return 1;
  }

  return 0;
}
