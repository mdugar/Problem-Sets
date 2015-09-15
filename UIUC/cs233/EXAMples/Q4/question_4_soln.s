# run with QtSpim -file main.s question_4_soln.s

# struct node_t {
#     node_t *next;
#     int *data;
# };
# void increment_linked(node_t *head, int value) {
#     for (node_t *curr = head; curr != NULL; curr = curr->next) {
#         *(curr->data) += value;
#     }
# }
.globl increment_linked
increment_linked:
inc_for:
    beq     $a0, $0, inc_end
    lw      $t0, 4($a0)
    lw      $t1, 0($t0)
    add     $t1, $t1, $a1
    sw      $t1, 0($t0)
    lw      $a0, 0($a0)
    j       inc_for
inc_end:
    jr      $ra
