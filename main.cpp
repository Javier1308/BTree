#include <iostream>
using namespace std;

struct Nodo
{
    int *keys;
    int g;
    Nodo **h;
    Nodo *next;
    int n;
    bool leaf;

    Nodo(int g, bool leaf)
    {
        this->g = g;
        this->leaf = leaf;
        keys = new int[(2 * g) - 1];
        h = new Nodo *[2 * g];
        n = 0;
        next = nullptr;
    }
};

class Arbol
{
public:
    Nodo *root;
    int g;

    Arbol(int g)
    {
        root = nullptr;
        this->g = g;
    }

    Nodo *find(Nodo *node, int k)
    {
        if (!node)
            return nullptr;
        int i = 0;
        while (i < node->n && k > node->keys[i])
            i++;

        if (node->leaf)
            return (i < node->n && node->keys[i] == k) ? node : nullptr;

        return find(node->h[i], k);
    }

    void add(int k)
    {
        if (!root)
        {
            root = new Nodo(g, true);
            root->keys[0] = k;
            root->n = 1;
        }
        else
        {
            if (root->n == (2 * g) - 1)
            {
                Nodo *newRoot = new Nodo(g, false);
                newRoot->h[0] = root;
                split(newRoot, 0, root);
                root = newRoot;
            }
            addNode(root, k);
        }
    }

    void addNode(Nodo *node, int k)
    {
        int i = node->n - 1;
        if (node->leaf)
        {
            while (i >= 0 && k < node->keys[i])
            {
                node->keys[i + 1] = node->keys[i];
                i--;
            }
            node->keys[i + 1] = k;
            node->n++;
        }
        else
        {
            while (i >= 0 && k < node->keys[i])
                i--;
            i++;
            if (node->h[i]->n == (2 * g) - 1)
            {
                split(node, i, node->h[i]);
                if (k > node->keys[i])
                    i++;
            }
            addNode(node->h[i], k);
        }
    }

    void split(Nodo *parent, int i, Nodo *child)
    {
        Nodo *newNode = new Nodo(g, child->leaf);
        newNode->n = g - 1;

        for (int j = 0; j < g - 1; j++)
            newNode->keys[j] = child->keys[j + g];

        if (!child->leaf)
        {
            for (int j = 0; j < g; j++)
                newNode->h[j] = child->h[j + g];
        }
        else
        {
            newNode->next = child->next;
            child->next = newNode;
        }

        child->n = g - 1;

        for (int j = parent->n; j >= i + 1; j--)
            parent->h[j + 1] = parent->h[j];

        parent->h[i + 1] = newNode;

        for (int j = parent->n - 1; j >= i; j--)
            parent->keys[j + 1] = parent->keys[j];

        parent->keys[i] = child->keys[g - 1];
        parent->n++;
    }

    void remove(int k)
    {
        if (!root)
        {
            cout << "Empty tree" << endl;
            return;
        }
        remKey(root, k);

        if (root->n == 0)
        {
            Nodo *tmp = root;
            root = root->leaf ? nullptr : root->h[0];
            delete tmp;
        }
    }

    void remKey(Nodo *node, int k)
    {
        int idx = findIdx(node, k);

        if (node->leaf)
        {
            if (idx < node->n && node->keys[idx] == k)
                remLeaf(node, idx);
            return;
        }

        if (idx < node->n && node->keys[idx] == k)
            remInner(node, idx);
        else
        {
            bool last = (idx == node->n);
            Nodo *child = node->h[idx];

            if (child->n < g)
                fill(node, idx);

            if (last && idx > node->n)
                remKey(node->h[idx - 1], k);
            else
                remKey(node->h[idx], k);
        }
    }

    void remLeaf(Nodo *node, int idx)
    {
        for (int i = idx + 1; i < node->n; ++i)
            node->keys[i - 1] = node->keys[i];
        node->n--;
    }

    void remInner(Nodo *node, int idx)
    {
        int k = node->keys[idx];

        if (node->h[idx]->n >= g)
        {
            int pred = getPred(node, idx);
            node->keys[idx] = pred;
            remKey(node->h[idx], pred);
        }
        else if (node->h[idx + 1]->n >= g)
        {
            int succ = getSucc(node, idx);
            node->keys[idx] = succ;
            remKey(node->h[idx + 1], succ);
        }
        else
        {
            merge(node, idx);
            remKey(node->h[idx], k);
        }
    }

    int getPred(Nodo *node, int idx)
    {
        Nodo *cur = node->h[idx];
        while (!cur->leaf)
            cur = cur->h[cur->n];
        return cur->keys[cur->n - 1];
    }

    int getSucc(Nodo *node, int idx)
    {
        Nodo *cur = node->h[idx + 1];
        while (!cur->leaf)
            cur = cur->h[0];
        return cur->keys[0];
    }

    void fill(Nodo *node, int idx)
    {
        if (idx != 0 && node->h[idx - 1]->n >= g)
            borrowPrev(node, idx);
        else if (idx != node->n && node->h[idx + 1]->n >= g)
            borrowNext(node, idx);
        else
        {
            if (idx != node->n)
                merge(node, idx);
            else
                merge(node, idx - 1);
        }
    }

    void borrowPrev(Nodo *node, int idx)
    {
        Nodo *child = node->h[idx];
        Nodo *sibling = node->h[idx - 1];

        for (int i = child->n - 1; i >= 0; --i)
            child->keys[i + 1] = child->keys[i];

        if (!child->leaf)
        {
            for (int i = child->n; i >= 0; --i)
                child->h[i + 1] = child->h[i];
        }

        child->keys[0] = node->keys[idx - 1];

        if (!node->leaf)
            child->h[0] = sibling->h[sibling->n];

        node->keys[idx - 1] = sibling->keys[sibling->n - 1];

        child->n += 1;
        sibling->n -= 1;
    }

    void borrowNext(Nodo *node, int idx)
    {
        Nodo *child = node->h[idx];
        Nodo *sibling = node->h[idx + 1];

        child->keys[child->n] = node->keys[idx];

        if (!(child->leaf))
            child->h[child->n + 1] = sibling->h[0];

        node->keys[idx] = sibling->keys[0];

        for (int i = 1; i < sibling->n; ++i)
            sibling->keys[i - 1] = sibling->keys[i];

        if (!sibling->leaf)
        {
            for (int i = 1; i <= sibling->n; ++i)
                sibling->h[i - 1] = sibling->h[i];
        }

        child->n += 1;
        sibling->n -= 1;
    }

    void merge(Nodo *node, int idx)
    {
        Nodo *child = node->h[idx];
        Nodo *sibling = node->h[idx + 1];

        child->keys[g - 1] = node->keys[idx];

        for (int i = 0; i < sibling->n; ++i)
            child->keys[i + g] = sibling->keys[i];

        if (!child->leaf)
        {
            for (int i = 0; i <= sibling->n; ++i)
                child->h[i + g] = sibling->h[i];
        }

        for (int i = idx + 1; i < node->n; ++i)
            node->keys[i - 1] = node->keys[i];

        for (int i = idx + 2; i <= node->n; ++i)
            node->h[i - 1] = node->h[i];

        child->n += sibling->n + 1;
        node->n--;

        delete sibling;
    }

    int findIdx(Nodo *node, int k)
    {
        int idx = 0;
        while (idx < node->n && node->keys[idx] < k)
            ++idx;
        return idx;
    }
};

int main()
{
    Arbol tree(3);
    tree.add(10);
    tree.add(20);
    tree.add(5);
    tree.add(6);
    tree.add(12);
    tree.add(30);
    tree.add(7);
    tree.add(17);

    cout << "Nodo encontrado: " << (tree.find(tree.root, 6) ? "Si" : "No") << endl;

    tree.remove(6);
    cout << "Nodo no encontrado: " << (tree.find(tree.root, 6) ? "Si" : "No") << endl;

    return 0;
}
