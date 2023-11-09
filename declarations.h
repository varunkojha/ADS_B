
/* Structure used to describe an aircraft in iteractive mode. */
typedef struct Aircraft 
{
    char flight[9];     /* Flight number */
    uint32_t addr;      /* ICAO address */
    char hexaddr[7];    /* Printable ICAO address */
    int altitude;       /* Altitude, feet or meters*/
    int velocity;       /* Velocity computed meter per second */
    int track;          /* Angle of flight. */
    time_t seen;        /* Time at which the last packet was received. */
    long messages;      /* Number of Mode S messages received. */
    double lat, lon;    /* Coordinated obtained from GNSS or GPS. */
    struct Aircraft *next; /* Next aircraft in our linked list. */
} Aircraft;

extern Aircraft *adsBIn, *adsBOut;

// Message Generator
Aircraft* generateMessage(Aircraft *);
Aircraft* (*msgG)(Aircraft *);

// Parsing Message
Aircraft* parseMessage(Aircraft *);
Aircraft* (*msgP)(Aircraft *);
