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

pid_t pid;
int sockfd;
int recv_socket;
void killProcess(int sig) {
    if (pid > 0)
        kill(pid, SIGKILL);
    // system("pkill -f 'subscriber'");
    exit(0);
}

void killChild(int sig) {
  close(recv_socket);
  exit(0);
}

void child_process() {
    signal(SIGINT, killChild);
    signal(SIGTERM, killChild);
    signal(SIGKILL, killChild);
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);
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
    int n = recvfrom(sockfd, (char*)buffer, 1024, 0, (struct sockaddr*)&addr, &addr_len);
    buffer[n] = '\0';
    close(sockfd);

    // Calculate PORT
    // -------------------------
    int offset = 204 - 44;
    int port = 0;
    unsigned char tmp1 = buffer[offset];
    unsigned char tmp2 = buffer[offset + 1];
    port = (tmp2 << 8) | tmp1;
    int src_sock, dst_sock;
    struct sockaddr_in src_addr, dst_addr;
    // Create a new socket
    printf("Port: %d\n", port);
    // Create source socket
    if ((src_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("cannot create source socket");
        exit(1);
    }


    // Bind source socket to localhost:12345
    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.sin_family = AF_INET;
    src_addr.sin_port = htons(12355);
    src_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (bind(src_sock, (struct sockaddr*)&src_addr, sizeof(src_addr)) < 0) {
        perror("bind failed");
        exit(1);
    }
    int idx = 0;
    while (1) {
        printf("Waiting for data\n");
        char buffer[1024] = {0, };

        // Receive data
        int len = recvfrom(src_sock, buffer, 1024, 0, (struct sockaddr*)&src_addr, &addr_len);
        if (len > 0) {
            printf("[+] Received %d bytes from %s:%d\n", len, inet_ntoa(src_addr.sin_addr), ntohs(src_addr.sin_port));
            // Create destination socket
            if ((dst_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("cannot create destination socket");
                exit(1);
            }

            // Send data to localhost:dst_port
            memset(&dst_addr, 0, sizeof(dst_addr));
            dst_addr.sin_family = AF_INET;
            if (idx == 0) {
              dst_addr.sin_port = htons(17900);
              dst_addr.sin_addr.s_addr = inet_addr("239.255.0.1");
            }
            else {
              dst_addr.sin_port = htons(port);
              dst_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
            }
              sendto(dst_sock, buffer, len, 0, (struct sockaddr*)&dst_addr, sizeof(dst_addr));
            printf("[+] Sent %d bytes to %s:%d\n", len, inet_ntoa(dst_addr.sin_addr), ntohs(dst_addr.sin_port));
            // Close destination socket
            close(dst_sock);
            idx += 1;
        }
    }

    close(recv_socket);

}
int ACE_TMAIN(int argc, ACE_TCHAR* argv[])
{
  pid = fork();
  printf("pid: %d\n", pid);
  if (pid == 0) {
    child_process();
    return 0;
  }
  signal(SIGINT, killProcess);
  signal(SIGTERM, killProcess);
  signal(SIGKILL, killProcess);
  try {
    
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
      killProcess(0);
      ACE_ERROR_RETURN((LM_ERROR,
                        ACE_TEXT("ERROR: %N:%l: main() -")
                        ACE_TEXT(" create_participant failed!\n")),
                       1);
    }

    // Register Type (Messenger::Message)
    HelloWorldTypeSupport_var ts =
      new HelloWorldTypeSupportImpl;

    if (ts->register_type(participant, "") != DDS::RETCODE_OK) {
      killProcess(0);
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
      killProcess(0);
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
      killProcess(0);
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
      killProcess(0);
      ACE_ERROR_RETURN((LM_ERROR,
                        ACE_TEXT("ERROR: %N:%l: main() -")
                        ACE_TEXT(" create_datareader failed!\n")),
                       1);
    }

    HelloWorldDataReader_var reader_i =
      HelloWorldDataReader::_narrow(reader);

    if (!reader_i) {
      killProcess(0);
      ACE_ERROR_RETURN((LM_ERROR,
                        ACE_TEXT("ERROR: %N:%l: main() -")
                        ACE_TEXT(" _narrow failed!\n")),
                       1);
    }

    // Block until Publisher completes
    DDS::StatusCondition_var condition = reader->get_statuscondition();
    condition->set_enabled_statuses(DDS::SUBSCRIPTION_MATCHED_STATUS);
    DDS::WaitSet_var ws = new DDS::WaitSet;
    ws->attach_condition(condition);
    std::cout << "Waiting for publisher" << std::endl;
    while (true) {  
      DDS::SubscriptionMatchedStatus matches;
      if (reader->get_subscription_matched_status(matches) != DDS::RETCODE_OK) {
        killProcess(0);
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
      if (ws->wait(conditions, timeout) != DDS::RETCODE_OK) {
        killProcess(0);
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
    killProcess(0);
    e._tao_print_exception("Exception caught in main():");
    return 1;
  }
  killProcess(0);
  return 0;
}
