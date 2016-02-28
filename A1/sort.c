// Quicksort algorithm referneced from:

//      zentut.com/c-tutorial/c-quicksort-algorithm/
// Bubblesort algorithm referenced from:
//      cprogramming.com/tutorial/computersciencetheory/sorting1.htmla

inline void swap(int *x, int *y) {
  int temp;

  temp = *x;
  *x   = *y;
  *y   = temp;
}

inline int choose_pivot(int i, int j) {
  return (i + j) / 2;
}

inline void quick_sort(int list[], int m, int n) {
  int key, i, j, k;

  if (m < n) {
    k = choose_pivot(m, n);
    swap(&list[m], &list[k]);
    key = list[m];
    i   = m + 1;
    j   = n;

    while (i <= j) {
      while ((i <= n) && (list[i] <= key)) i++;

      while ((j >= m) && (list[j] > key)) j--;

      if (i < j) swap(&list[i], &list[j]);
    }
    swap(&list[m], &list[j]);
    quick_sort(list, m,     j - 1);
    quick_sort(list, j + 1, n);
  }
}

inline void bubble_sort(int a[], const int size)
{
  int i, j;

  for (i = 0; i < (size - 1); i++) {
    for (j = 0; j < (size - (i + 1)); j++) {
      if (a[j] > a[j + 1]) swap(&a[j], &a[j + 1]);
    }
  }
}

