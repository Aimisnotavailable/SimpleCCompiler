/* ------------------------------------------------------------------
   AST visualisation – Graphviz DOT format
   ------------------------------------------------------------------ */
static int node_id_counter = 0;

static void ast_to_dot_rec(FILE *f, ASTNode *n, int parent_id) {
    if (!n) return;
    int my_id = node_id_counter++;
    const char *shape = "ellipse";
    char label[256] = "";

    switch (n->kind) {
        case NODE_INT:
            snprintf(label, sizeof(label), "INT(%d)", n->d.ival);
            shape = "box";
            break;
        case NODE_FLOAT:
            snprintf(label, sizeof(label), "FLOAT(%g)", n->d.fval);
            shape = "box";
            break;
        case NODE_ID:
            snprintf(label, sizeof(label), "ID(%s)", n->d.sval);
            shape = "box";
            break;
        case NODE_ADD:
            snprintf(label, sizeof(label), "ADD");
            break;
        case NODE_LT:
            snprintf(label, sizeof(label), "LT");
            break;
        case NODE_ASSIGN:
            snprintf(label, sizeof(label), "ASSIGN(%s)", n->d.assign.name);
            break;
        case NODE_PRINTF:
            snprintf(label, sizeof(label), "PRINTF(%s)", n->d.print.fmt);
            shape = "diamond";
            break;
        case NODE_BLOCK:
            snprintf(label, sizeof(label), "BLOCK[%d]", n->d.block.cnt);
            shape = "tab";
            break;
        case NODE_LOOP:
            snprintf(label, sizeof(label), "DO-WHILE");
            shape = "house";
            break;
        default:
            snprintf(label, sizeof(label), "???");
            break;
    }

    fprintf(f, "  n%d [label=\"%s\", shape=%s];\n", my_id, label, shape);
    if (parent_id >= 0)
        fprintf(f, "  n%d -> n%d;\n", parent_id, my_id);

    /* Add children according to node kind */
    switch (n->kind) {
        case NODE_ADD:
        case NODE_LT:
            ast_to_dot_rec(f, n->d.bin.left, my_id);
            ast_to_dot_rec(f, n->d.bin.right, my_id);
            break;
        case NODE_ASSIGN:
            ast_to_dot_rec(f, n->d.assign.expr, my_id);
            break;
        case NODE_PRINTF:
            ast_to_dot_rec(f, n->d.print.expr, my_id);
            break;
        case NODE_BLOCK:
            for (int i = 0; i < n->d.block.cnt; i++)
                ast_to_dot_rec(f, n->d.block.stmts[i], my_id);
            break;
        case NODE_LOOP:
            ast_to_dot_rec(f, n->d.loop.body, my_id);
            ast_to_dot_rec(f, n->d.loop.cond, my_id);
            break;
        default:
            break;  /* leaf nodes */
    }
}

void generate_ast_dot(ASTNode *root, const char *filename) {
    if (!root) return;
    FILE *f = fopen(filename, "w");
    if (!f) {
        perror(filename);
        return;
    }
    node_id_counter = 0;
    fprintf(f, "digraph AST {\n");
    fprintf(f, "  node [fontname=\"Courier\"];\n");
    ast_to_dot_rec(f, root, -1);
    fprintf(f, "}\n");
    fclose(f);
    printf("AST written to %s\n", filename);
}