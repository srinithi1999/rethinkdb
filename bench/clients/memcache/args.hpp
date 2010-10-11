
#ifndef __ARGS_HPP__
#define __ARGS_HPP__

/* Usage */
void usage(const char *name) {
    // Create a default config
    config_t _d;

    // Print usage
    printf("Usage:\n");
    printf("\t%s [OPTIONS]\n", name);

    printf("\nOptions:\n");
    printf("\t-n, --host\n\t\tServer host to connect to. Defaults to [%s].\n", _d.host);
    printf("\t-p, --port\n\t\tServer port to connect to. Defaults to [%d].\n", _d.port);
    printf("\t-c, --clients\n\t\tNumber of concurrent clients. Defaults to [%d].\n", _d.clients);
    printf("\t-w, --workload\n\t\tTarget load to generate. Expects a value in format D/U/I/R, where\n" \
           "\t\t\tD - number of deletes\n" \
           "\t\t\tU - number of updates\n" \
           "\t\t\tI - number of inserts\n" \
           "\t\t\tR - number of reads\n" \
           "\t\tDefaults to [");
    _d.load.print();
    printf("]\n");
    printf("\t-k, --keys\n\t\tKey distribution in DISTR format (see below). Defaults to [");
    _d.keys.print();
    printf("].\n");
    printf("\t-v, --values\n\t\tValue distribution in DISTR format (see below). Defaults to [");
    _d.values.print();
    printf("].\n");
    printf("\t-d, --duration\n\t\tDuration of the run specified in number of operations.\n" \
           "\t\tDefaults to [%ld].\n", _d.duration);
    printf("\t-l, --latency-file\n\t\tFile name to output individual latency information (in us).\n" \
           "\t\tThe information is not outputted if this argument is skipped.\n");
    printf("\t-q, --qps-file\n\t\tFile name to output QPS information.\n" \
           "\t\tThe information is not outputted if this argument is skipped.\n");

    printf("\nAdditional information:\n");
    printf("\t\tDISTR format describes a range and can be specified in as MIN-MAX\n");
    
    exit(-1);
}

/* Parse the args */
void parse(config_t *config, int argc, char *argv[]) {
    optind = 1; // reinit getopt
    while(1)
    {
        int do_help = 0;
        struct option long_options[] =
            {
                {"host",           required_argument, 0, 'n'},
                {"port",           required_argument, 0, 'p'},
                {"clients",        required_argument, 0, 'c'},
                {"workload",       required_argument, 0, 'w'},
                {"keys",           required_argument, 0, 'k'},
                {"values",         required_argument, 0, 'v'},
                {"duration",       required_argument, 0, 'd'},
                {"latency-file",   required_argument, 0, 'l'},
                {"qps-file",       required_argument, 0, 'q'},
                {"help",           no_argument, &do_help, 1},
                {0, 0, 0, 0}
            };

        int option_index = 0;
        int c = getopt_long(argc, argv, "n:p:c:w:k:v:d:l:q:h", long_options, &option_index);

        if(do_help)
            c = 'h';
     
        /* Detect the end of the options. */
        if (c == -1)
            break;
     
        switch (c)
        {
        case 0:
            break;
        case 'n':
            strncpy(config->host, optarg, MAX_HOST);
            break;
        case 'p':
            config->port = atoi(optarg);
            break;
        case 'c':
            config->clients = atoi(optarg);
            break;
        case 'w':
            config->load.parse(optarg);
            break;
        case 'k':
            config->keys.parse(optarg);
            break;
        case 'v':
            config->values.parse(optarg);
            break;
        case 'd':
            config->duration = atol(optarg);
            break;
        case 'l':
            strncpy(config->latency_file, optarg, MAX_FILE);
            break;
        case 'q':
            strncpy(config->qps_file, optarg, MAX_FILE);
            break;
        case 'h':
            usage(argv[0]);
            break;
     
        default:
            /* getopt_long already printed an error message. */
            usage(argv[0]);
        }
    }
}

#endif // __ARGS_HPP__

