#include <bits/stdc++.h>

#include "tests.h"
#include "deletion.h"
using namespace std;

int get_height(rb_node *node){
    if(node == nullptr){
        return -1;
    }
    int left = get_height(node->left_child);
    int right = get_height(node->right_child);

    if(left > right){ return left +1;}
    return right + 1;

}
// this function modified from
// https://stackoverflow.com/questions/36802354/print-binary-tree-in-a-pretty-way-using-c
void print_rb_tree(const string& prefix, const rb_node* node, bool isLeft)
{
    if( node != nullptr )
    {
        std::cout << prefix;

        std::cout << (isLeft ? "├──" : "└──" );

        // print the value of the node
        std::cout << node->value << '.' << node->color << std::endl;

        // enter the next tree level - left and right branch
        print_rb_tree(prefix + (isLeft ? "│   " : "    "), node->left_child, true);
        print_rb_tree(prefix + (isLeft ? "│   " : "    "), node->right_child, false);
    }
}

void init_node(rb_node *node, int value){
    node->color = 1;
    node->parent = nullptr;
    node->left_child = nullptr;
    node->right_child = nullptr;
    node->value = value;

}

rb_node* find_node_to_connect(tree* rb_tree, int value){
    assert(rb_tree != nullptr);
    rb_node *temp = rb_tree->root;

    while(temp != nullptr){
        if(temp->value > value && temp->left_child == nullptr){
            break;
        }
        if(temp->value <= value && temp->right_child == nullptr){
            break;
        }
        if(temp->value > value){
            temp = temp->left_child;
        }
        else{
            temp = temp->right_child;
        }
    }
    return temp;

}

// if rotation true => clockwise
void fix_line(tree* rb_tree, rb_node* node, bool rotation){
    rb_node *grand_parent = node->parent->parent;
    rb_node *temp = nullptr;

    // change colors of parents and grandparent
    if(grand_parent->color == 2){
        grand_parent->color = 1;
    }
    else{
        grand_parent->color = 2;
    }

    if(node->parent->color == 2){
        node->parent->color = 1;
    } else{
        node->parent->color = 2;
    }

    if(grand_parent->parent != nullptr) { // if not root

        if (grand_parent->parent->value > node->parent->value) { // grandparent is left child of his parent
            grand_parent->parent->left_child = node->parent;

        } else {
            grand_parent->parent->right_child = node->parent;
        }
    }
    else{
        rb_tree->root = node->parent;
    }
    node->parent->parent = grand_parent->parent;
    grand_parent->parent = node->parent;

    if(rotation){ // right rotation

        if(node->parent->right_child != nullptr){
            temp = node->parent->right_child;
            temp->parent = grand_parent;
        }
        node->parent->right_child = grand_parent;
        grand_parent->left_child = temp;
    }
    else{ // left rotation
        if(node->parent->left_child != nullptr){
            temp = node->parent->left_child;
            temp->parent = grand_parent;
        }
        node->parent->left_child = grand_parent;
        grand_parent->right_child = temp;
    }
}

void fix_triangle(tree* rb_tree, rb_node* node, bool rotation_right){
    assert(node != nullptr);

    rb_node *grand_parent = node->parent->parent;
    rb_node *temp = nullptr;


    if(rotation_right){
        if(node->right_child != nullptr){
            temp = node->right_child;
            node->right_child->parent = node->parent;
        }
        node->parent->left_child = temp;

        node->right_child = node->parent;
        node->parent->parent = node;
        node->parent = grand_parent;
        grand_parent->right_child = node;
    }
    else{ // left
        if(node->left_child != nullptr){
            temp = node->left_child;
            node->left_child->parent = node->parent;
        }
        node->parent->right_child = temp;

        node->left_child = node->parent;
        node->parent->parent = node;
        node->parent = grand_parent;
        grand_parent->left_child = node;
    }
}

void fix_case_one(tree* rb_tree, rb_node* node, bool right){
    assert(node != nullptr);
    rb_node *grand_parent = node->parent->parent;

    if(node->parent->color == 1){
        node->parent->color = 2;
    }
    else{
        node->parent->color = 1;
    }

    if(grand_parent->color == 1){
        grand_parent->color = 2;
    }
    else{
        grand_parent->color = 1;
    }

    if(grand_parent->parent == nullptr){ // if root then must be black
        grand_parent->color = 2;
    }

    if(right){
        grand_parent->right_child->color = 2;
    }
    else{
        grand_parent->left_child->color = 2;
    }
}


void fix(tree* rb_tree, rb_node* node){
    if(node == nullptr){
        return;
    }
    //dfs
    if(node->left_child != nullptr){
        fix(rb_tree, node->left_child);
    }
    if(node->right_child != nullptr){
        fix(rb_tree, node->right_child);
    }

    if(node->parent == nullptr){ // only root
        return;
    }
    // only first level
    if(node->parent->parent == nullptr){
        return;
    }
    rb_node *grand_parent = node->parent->parent;

    if(node->color == 1 && node->parent->color == 1 && grand_parent->color == 2){ // 2 red and grand.p black
        // case 3(line): uncle is black or NULL and grandparent is black, node and parent red
        if(node->value < node->parent->value && node->parent->value < grand_parent->value && // check if left line
                (grand_parent->right_child == nullptr || grand_parent->right_child->color == 2)){ // rotate line to right
            fix_line(rb_tree, node, true);
            return;
        }

        if(node->value >= node->parent->value  && node->parent->value >= grand_parent->value && // right line to left
                (grand_parent->left_child == nullptr || grand_parent->left_child->color == 2)){
            fix_line(rb_tree, node, false);
        }
        // case 2 triangle: only rotate
        if(node->value < node->parent->value && node->parent->value >= grand_parent->value &&  // rotate right triangle
             (grand_parent->left_child == nullptr || grand_parent->left_child->color == 2)){
            fix_triangle(rb_tree,node, true);
            return;
        }

        if(node->value >= node->parent->value && node->parent->value < grand_parent->value &&  // rotate left triangle
           (grand_parent->right_child == nullptr || grand_parent->right_child->color == 2)){
            fix_triangle(rb_tree,node, false);
        }
    }
    //case 1: uncle is red
    if(grand_parent->right_child != nullptr&& grand_parent->right_child->color == 1 && node->parent->color == 1
      && node->color == 1 && grand_parent->value <= grand_parent->right_child->value && node->value < grand_parent->value){ // right uncle
        fix_case_one(rb_tree, node, true);
    }

    if(grand_parent->left_child != nullptr && grand_parent->left_child->color == 1 && node->parent->color == 1
    && node->color == 1 && grand_parent->value > grand_parent->left_child->value && node->value >= grand_parent->value){ // left uncle
        fix_case_one(rb_tree, node, false);

    }

}

void insert_node(tree* rb_tree, int value){
    assert(rb_tree != nullptr);

    rb_node* node = nullptr;
    node = (struct rb_node*)malloc(sizeof(rb_node));

    if(node == nullptr){ // if alloc fails
        printf("Could not allocate memory!");
        exit(-1);
    }
    // initialize node
    init_node(node, value);

    if(rb_tree->root == nullptr){
        node->color = 2;
        rb_tree->root = node;
        return;
    }

    rb_node *last_node = find_node_to_connect(rb_tree, value);
    if(last_node->value <= value){ // right child, equal or higher
        last_node->right_child = node;
    }
    else{
        last_node->left_child = node;
    }

    node->parent = last_node;
    fix(rb_tree, rb_tree->root);
}

int main(int argc,char **argv){
    //uncomment if you want to run tests
    //run_tests(argc, argv);


    cout<< "...................................." << endl;
    tree rb_tree2;
    rb_tree2.root = nullptr;

    insert_node(&rb_tree2, 15);
    insert_node(&rb_tree2, 20);
    insert_node(&rb_tree2, 25);
    insert_node(&rb_tree2, 9);
    insert_node(&rb_tree2, 30);
    insert_node(&rb_tree2, 35);
    insert_node(&rb_tree2, 14);
    insert_node(&rb_tree2, 17);
    insert_node(&rb_tree2, 33);
    insert_node(&rb_tree2, 45);
    insert_node(&rb_tree2, 8);
    insert_node(&rb_tree2, 7);
    insert_node(&rb_tree2, 6);

    cout<< "...................................." << endl;
    print_rb_tree("",rb_tree2.root, false);

    main_deletion_switcher(&rb_tree2, 25);
    cout << "25" << endl;
    print_rb_tree("",rb_tree2.root, false);
    main_deletion_switcher(&rb_tree2, 45);
    cout << "45" << endl;
    print_rb_tree("",rb_tree2.root, false);
    main_deletion_switcher(&rb_tree2, 30);
    cout << "30 was deleted" << endl;
    print_rb_tree("",rb_tree2.root, false);
    main_deletion_switcher(&rb_tree2, 9);
    cout << "9 was deleted" << endl;
    print_rb_tree("",rb_tree2.root, false);

    main_deletion_switcher(&rb_tree2, 8);
    cout << "8 was deleted" << endl;
    print_rb_tree("",rb_tree2.root, false);

    main_deletion_switcher(&rb_tree2, 7);
    cout << "7 was deleted" << endl;
    print_rb_tree("",rb_tree2.root, false);

    main_deletion_switcher(&rb_tree2, 6); // case 2
    cout << "6 was deleted" << endl;
    print_rb_tree("",rb_tree2.root, false);
    /////////////////////////////////////////////////////////////

    main_deletion_switcher(&rb_tree2, 20); // case 2
    cout << "20 was deleted" << endl;
    print_rb_tree("",rb_tree2.root, false);

    main_deletion_switcher(&rb_tree2, 33); //case 2
    cout << "33 was deleted" << endl;
    print_rb_tree("",rb_tree2.root, false);

    main_deletion_switcher(&rb_tree2, 15);
    print_rb_tree("",rb_tree2.root, false);

    main_deletion_switcher(&rb_tree2, 14);
    print_rb_tree("",rb_tree2.root, false);

    main_deletion_switcher(&rb_tree2, 35);
    print_rb_tree("",rb_tree2.root, false);

    main_deletion_switcher(&rb_tree2, 17);
    print_rb_tree("",rb_tree2.root, false);
    return 0;
}
