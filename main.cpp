#include <iostream>

using namespace std;

struct Node
{
    int *keys;
    int t;
    Node **childPointers;
    int n;
    bool leaf;
};

class BTree
{
public:
    Node *root;
    int t;

    BTree(int degree)
    {
        root = nullptr;
        t = degree;
    }

    // Crear un nuevo nodo
    Node *createNode(bool leaf)
    {
        Node *newNode = new Node;
        newNode->keys = new int[(2 * t) - 1];
        newNode->childPointers = new Node *[2 * t];
        newNode->leaf = leaf;
        newNode->n = 0;
        return newNode;
    }

    // Buscar una llave en el BTree
    Node *search(Node *node, int key)
    {
        int i = 0;
        while (i < node->n && key > node->keys[i])
        {
            i++;
        }
        if (node->keys[i] == key)
        {
            return node;
        }
        if (node->leaf)
        {
            return nullptr;
        }
        return search(node->childPointers[i], key);
    }

    // Insertar una llave en el BTree
    void insert(int key)
    {
        if (root == nullptr)
        {
            root = createNode(true);
            root->keys[0] = key;
            root->n = 1;
        }
        else
        {
            if (root->n == (2 * t) - 1)
            {
                Node *newRoot = createNode(false);
                newRoot->childPointers[0] = root;
                splitChild(newRoot, 0, root);
                insertNonFull(newRoot, key);
                root = newRoot;
            }
            else
            {
                insertNonFull(root, key);
            }
        }
    }

    // Insertar en un nodo que no está lleno
    void insertNonFull(Node *node, int key)
    {
        int i = node->n - 1;
        if (node->leaf)
        {
            while (i >= 0 && key < node->keys[i])
            {
                node->keys[i + 1] = node->keys[i];
                i--;
            }
            node->keys[i + 1] = key;
            node->n++;
        }
        else
        {
            while (i >= 0 && key < node->keys[i])
            {
                i--;
            }
            i++;
            if (node->childPointers[i]->n == (2 * t) - 1)
            {
                splitChild(node, i, node->childPointers[i]);
                if (key > node->keys[i])
                {
                    i++;
                }
            }
            insertNonFull(node->childPointers[i], key);
        }
    }

    // Dividir un hijo de un nodo
    void splitChild(Node *parentNode, int childIndex, Node *childNode)
    {
        Node *newNode = createNode(childNode->leaf);
        newNode->n = t - 1;
        for (int j = 0; j < t - 1; j++)
        {
            newNode->keys[j] = childNode->keys[j + t];
        }
        if (!childNode->leaf)
        {
            for (int j = 0; j < t; j++)
            {
                newNode->childPointers[j] = childNode->childPointers[j + t];
            }
        }
        childNode->n = t - 1;
        for (int j = parentNode->n; j >= childIndex + 1; j--)
        {
            parentNode->childPointers[j + 1] = parentNode->childPointers[j];
        }
        parentNode->childPointers[childIndex + 1] = newNode;
        for (int j = parentNode->n - 1; j >= childIndex; j--)
        {
            parentNode->keys[j + 1] = parentNode->keys[j];
        }
        parentNode->keys[childIndex] = childNode->keys[t - 1];
        parentNode->n++;
    }

    // Eliminar una llave del BTree
    void remove(int key)
    {
        if (root == nullptr)
        {
            return;
        }
        removeKey(root, key);
        if (root->n == 0)
        {
            Node *oldRoot = root;
            if (root->leaf)
            {
                root = nullptr;
            }
            else
            {
                root = root->childPointers[0];
            }
            delete oldRoot;
        }
    }

    // Eliminar una llave de un nodo
    void removeKey(Node *node, int key)
    {
        int index = findKeyIndex(node, key);
        if (index < node->n && node->keys[index] == key)
        {
            if (node->leaf)
            {
                removeKeyFromLeaf(node, index);
            }
            else
            {
                removeKeyFromNonLeaf(node, index);
            }
        }
        else
        {
            if (node->leaf)
            {
                return;
            }
            bool isLastChild = (index == node->n);
            Node *childNode = node->childPointers[index];
            if (childNode->n < t)
            {
                fillChildNode(node, index);
            }
            if (isLastChild && index > node->n)
            {
                removeKey(childNode, key);
            }
            else
            {
                removeKey(childNode, key);
            }
        }
    }

    // Encontrar el index de una llave en un nodo
    int findKeyIndex(Node *node, int key)
    {
        int index = 0;
        while (index < node->n && key > node->keys[index])
        {
            index++;
        }
        return index;
    }

    // Eliminar una llave de un leaf
    void removeKeyFromLeaf(Node *node, int index)
    {
        for (int i = index + 1; i < node->n; i++)
        {
            node->keys[i - 1] = node->keys[i];
        }
        node->n--;
    }

    // Eliminar una llave de un nodo no leaf
    void removeKeyFromNonLeaf(Node *node, int index)
    {
        int key = node->keys[index];
        Node *predecessor = node->childPointers[index];
        Node *successor = node->childPointers[index + 1];
        if (predecessor->n >= t)
        {
            int predecessorKey = getPredecessor(predecessor);
            node->keys[index] = predecessorKey;
            removeKey(predecessor, predecessorKey);
        }
        else if (successor->n >= t)
        {
            int successorKey = getSuccessor(successor);
            node->keys[index] = successorKey;
            removeKey(successor, successorKey);
        }
        else
        {
            mergeNodes(node, index, predecessor, successor);
            removeKey(predecessor, key);
        }
    }

    // Obtener el predecesor de un nodo
    int getPredecessor(Node *node)
    {
        while (!node->leaf)
        {
            node = node->childPointers[node->n];
        }
        return node->keys[node->n - 1];
    }

    // Obtener el sucesor de un nodo
    int getSuccessor(Node *node)
    {
        while (!node->leaf)
        {
            node = node->childPointers[0];
        }
        return node->keys[0];
    }

    // Llenar un childnode que tiene menos de t llaves
    void fillChildNode(Node *node, int index)
    {
        Node *childNode = node->childPointers[index];
        Node *leftSibling = (index != 0) ? node->childPointers[index - 1] : nullptr;
        Node *rightSibling = (index != node->n) ? node->childPointers[index + 1] : nullptr;
        if (leftSibling && leftSibling->n >= t)
        {
            borrowFromLeftSibling(node, index, childNode, leftSibling);
        }
        else if (rightSibling && rightSibling->n >= t)
        {
            borrowFromRightSibling(node, index, childNode, rightSibling);
        }
        else
        {
            if (leftSibling)
            {
                mergeNodes(node, index - 1, leftSibling, childNode);
            }
            else
            {
                mergeNodes(node, index, childNode, rightSibling);
            }
        }
    }

    // Tomar prestada una llave de un hermano izquierdo
    void borrowFromLeftSibling(Node *node, int index, Node *childNode, Node *leftSibling)
    {
        for (int i = childNode->n - 1; i >= 0; i--)
        {
            childNode->keys[i + 1] = childNode->keys[i];
        }
        if (!childNode->leaf)
        {
            for (int i = childNode->n; i >= 0; i--)
            {
                childNode->childPointers[i + 1] = childNode->childPointers[i];
            }
        }
        childNode->keys[0] = node->keys[index - 1];
        if (!childNode->leaf)
        {
            childNode->childPointers[0] = leftSibling->childPointers[leftSibling->n];
        }
        node->keys[index - 1] = leftSibling->keys[leftSibling->n - 1];
        childNode->n++;
        leftSibling->n--;
    }

    // Tomar prestada una llave de un hermano derecho
    void borrowFromRightSibling(Node *node, int index, Node *childNode, Node *rightSibling)
    {
        childNode->keys[childNode->n] = node->keys[index];
        if (!childNode->leaf)
        {
            childNode->childPointers[childNode->n + 1] = rightSibling->childPointers[0];
        }
        node->keys[index] = rightSibling->keys[0];
        for (int i = 1; i < rightSibling->n; i++)
        {
            rightSibling->keys[i - 1] = rightSibling->keys[i];
        }
        if (!rightSibling->leaf)
        {
            for (int i = 1; i <= rightSibling->n; i++)
            {
                rightSibling->childPointers[i - 1] = rightSibling->childPointers[i];
            }
        }
        childNode->n++;
        rightSibling->n--;
    }

    // Fusionar dos nodos
    void mergeNodes(Node *parentNode, int index, Node *leftNode, Node *rightNode)
    {
        leftNode->keys[leftNode->n] = parentNode->keys[index];
        for (int i = 0; i < rightNode->n; i++)
        {
            leftNode->keys[leftNode->n + 1 + i] = rightNode->keys[i];
        }
        if (!leftNode->leaf)
        {
            for (int i = 0; i <= rightNode->n; i++)
            {
                leftNode->childPointers[leftNode->n + 1 + i] = rightNode->childPointers[i];
            }
        }
        for (int i = index + 1; i < parentNode->n; i++)
        {
            parentNode->keys[i - 1] = parentNode->keys[i];
        }
        for (int i = index + 2; i <= parentNode->n; i++)
        {
            parentNode->childPointers[i - 1] = parentNode->childPointers[i];
        }
        leftNode->n += rightNode->n + 1;
        parentNode->n--;
        delete rightNode;
    }
};

int main()
{
    BTree bTree(3); // Crear un BTree con grado 3

    // Insertar claves en el BTree
    bTree.insert(10);
    bTree.insert(20);
    bTree.insert(30);
    bTree.insert(40);
    bTree.insert(50);

    // Buscar una clave en el BTree
    Node *result = bTree.search(bTree.root, 30);
    if (result != nullptr)
    {
        cout << "Clave encontrada en el BTree" << endl;
    }
    else
    {
        cout << "Clave no encontrada en el BTree" << endl;
    }

    // Eliminar una clave del BTree
    bTree.remove(30);

    return 0;
}
