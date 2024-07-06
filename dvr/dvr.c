#include <stdio.h>
int main()
{
    int no_of_nodes;
    printf("Enter no of nodes : ");
    scanf("%d", &no_of_nodes);
    int routing_table[no_of_nodes][no_of_nodes];
    printf("Enter the routing table:\n");
    for (int i = 0; i < no_of_nodes; i++) {
        for (int j = 0; j < no_of_nodes; j++) {
            printf("Value for %d -> %d: ", i, j);
            scanf("%d", &routing_table[i][j]);
        }
    }

    for (int i = 0; i < no_of_nodes; i++) {

        for (int j = 0; j < no_of_nodes; j++) {

            for (int k = 0; k < no_of_nodes; k++) {

                if (routing_table[i][j] > routing_table[i][k] + routing_table[k][j]) {
                    routing_table[i][j] = routing_table[i][k] + routing_table[k][j];
                }
            }
        }
    }

    printf("Updated Table:\n");
    for (int i = 0; i < no_of_nodes; i++) {
        for (int j = 0; j < no_of_nodes; j++) {
            printf("%d  ", routing_table[i][j]);
        }
        printf("\n");
    }
    return 0;
}
