

#include "ConfigParser.hpp"

#include "Cluster.hpp"
#include "Client.hpp"
#include <cstring>
#include <csignal>

void hand(int, siginfo_t *, void *);

int main(int argc, char **argv)
{
    if (argc > 2) {
        PRINTUSAGE; 
        return 1;
    }

    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = NULL;
    act.sa_sigaction = hand;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &act, NULL);

    try {
        Cluster cluster(argc > 1 ? argv[1] : "./config/default.conf");
        cluster.runCluster();
    }
    catch(const std::exception& e) {
        std::cerr	<< YELLOW << e.what() << std::endl
                    << RED "webserv : EXIT_FAILURE"
					<< RESET << std::endl;
        return 1;
    }
    return 0;
}
