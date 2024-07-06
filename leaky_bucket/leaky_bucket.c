#include <stdio.h>

int main() {
    int bucket_size, outgoing_rate, no_of_inputs;
    int incoming, store = 0;

    printf("Enter Bucket size: ");
    scanf("%d", &bucket_size);

    printf("Enter Outgoing Rate: ");
    scanf("%d", &outgoing_rate);

    printf("Enter number of inputs: ");
    scanf("%d", &no_of_inputs);

    while (no_of_inputs > 0) {
        printf("Enter the packet size: ");
        scanf("%d", &incoming);

        if (incoming <= (bucket_size - store)) {
            store += incoming;
            printf("Bucket buffer size is %d out of %d\n", store, bucket_size);
        } else {
            int drop = incoming - (bucket_size - store);
            store = bucket_size;
            printf("Dropped %d packets\n", drop);
            printf("Bucket buffer size is %d out of %d\n", store, bucket_size);
        }

        store -= outgoing_rate;
        if (store < 0) {
            store = 0;
        }
        printf("After outgoing, %d packets left out of %d in buffer\n", store, bucket_size);
        no_of_inputs--;
    }
    return 0;
}
