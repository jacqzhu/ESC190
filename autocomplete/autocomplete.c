#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "autocomplete.h"

int compare_terms(const void *a, const void *b) {
    const term *term1 = (const term *)a;
    const term *term2 = (const term *)b;
    return strcmp(term1->term, term2->term);
}

void read_in_terms(struct term **terms, int *pnterms, char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(1);
    }

    fscanf(file, "%d\n", pnterms);  // read number of terms

    *terms = (term *)malloc((*pnterms) * sizeof(term));
    if (*terms == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        exit(1);
    }

    for (int i = 0; i < *pnterms; i++) {
        fscanf(file, "%lf\t%[^\n]\n", &(*terms)[i].weight, (*terms)[i].term);
    }

    qsort(*terms, *pnterms, sizeof(term), compare_terms);

    fclose(file);
}

int lowest_match(struct term *terms, int nterms, char *substr) {
    int low = 0, high = nterms - 1, first = -1;
    size_t len = strlen(substr);

    while (low <= high) {
        int mid = (low + high) / 2;
        int cmp = strncmp(terms[mid].term, substr, len);
        if (cmp == 0) {
            first = mid;
            high = mid - 1;
        } else if (cmp < 0) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return first;
}

int highest_match(struct term *terms, int nterms, char *substr) {
    int low = 0, high = nterms - 1, last = -1;
    size_t len = strlen(substr);

    while (low <= high) {
        int mid = (low + high) / 2;
        int cmp = strncmp(terms[mid].term, substr, len);
        if (cmp == 0) {
            last = mid;
            low = mid + 1;
        } else if (cmp < 0) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return last;
}

int compare_weight(const void *a, const void *b) {
    const term *term1 = (const term *)a;
    const term *term2 = (const term *)b;
    if (term1->weight < term2->weight) return 1;
    else if (term1->weight > term2->weight) return -1;
    else return 0;
}

void autocomplete(struct term **answer, int *n_answer, struct term *terms, int nterms, char *substr) {
    int first_match = lowest_match(terms, nterms, substr);
    int last_match = highest_match(terms, nterms, substr);

    if (first_match == -1 || last_match == -1 || last_match < first_match) {
        *n_answer = 0;
        *answer = NULL;
        return;
    }

    *n_answer = last_match - first_match + 1;
    *answer = (term *)malloc((*n_answer) * sizeof(term));
    if (*answer == NULL) {
        fprintf(stderr, "Memory allocation failed in autocomplete\n");
        exit(1);
    }

    for (int i = 0; i < *n_answer; i++) {
        (*answer)[i] = terms[first_match + i];
    }

    qsort(*answer, *n_answer, sizeof(term), compare_weight);
}
