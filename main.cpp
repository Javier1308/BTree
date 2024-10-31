#include <iostream>
#include <vector>

using namespace std;

struct Nodo
{
    int *claves;
    int grado;
    Nodo **punterosHijos;
    int n;
    bool hoja;
    Nodo *siguiente;

    Nodo(int gradoNodo, bool esHoja)
    {
        grado = gradoNodo;
        hoja = esHoja;
        claves = new int[2 * grado - 1];
        punterosHijos = new Nodo *[2 * grado];
        n = 0;
        siguiente = nullptr;
    }
};

class BPlusTree
{
public:
    Nodo *raiz;
    int grado;

    BPlusTree(int gradoArbol)
    {
        raiz = nullptr;
        grado = gradoArbol;
    }

    Nodo *buscar(int clave)
    {
        return buscar(raiz, clave);
    }

    Nodo *buscar(Nodo *nodo, int clave)
    {
        int i = 0;
        while (i < nodo->n && clave > nodo->claves[i])
        {
            i++;
        }

        if (nodo->hoja)
        {
            if (i < nodo->n && nodo->claves[i] == clave)
            {
                return nodo;
            }
            else
            {
                return nullptr;
            }
        }
        else
        {
            return buscar(nodo->punterosHijos[i], clave);
        }
    }

    void insertar(int clave)
    {
        if (raiz == nullptr)
        {
            raiz = new Nodo(grado, true);
            raiz->claves[0] = clave;
            raiz->n = 1;
        }
        else
        {
            if (raiz->n == 2 * grado - 1)
            {
                Nodo *nuevaRaiz = new Nodo(grado, false);
                nuevaRaiz->punterosHijos[0] = raiz;
                dividirHijo(nuevaRaiz, 0, raiz);
                int i = 0;
                if (nuevaRaiz->claves[0] < clave)
                {
                    i++;
                }
                insertarNoLleno(nuevaRaiz->punterosHijos[i], clave);
                raiz = nuevaRaiz;
            }
            else
            {
                insertarNoLleno(raiz, clave);
            }
        }
    }

    void insertarNoLleno(Nodo *nodo, int clave)
    {
        int i = nodo->n - 1;

        if (nodo->hoja)
        {
            while (i >= 0 && clave < nodo->claves[i])
            {
                nodo->claves[i + 1] = nodo->claves[i];
                i--;
            }
            nodo->claves[i + 1] = clave;
            nodo->n++;

            if (nodo->n == 2 * grado - 1)
            {
                if (nodo == raiz)
                {
                    Nodo *nuevaRaiz = new Nodo(grado, false);
                    nuevaRaiz->punterosHijos[0] = raiz;
                    dividirHijo(nuevaRaiz, 0, raiz);
                    raiz = nuevaRaiz;
                }
            }
        }
        else
        {
            while (i >= 0 && clave < nodo->claves[i])
            {
                i--;
            }
            i++;
            if (nodo->punterosHijos[i]->n == 2 * grado - 1)
            {
                dividirHijo(nodo, i, nodo->punterosHijos[i]);
                if (clave > nodo->claves[i])
                {
                    i++;
                }
            }
            insertarNoLleno(nodo->punterosHijos[i], clave);
        }
    }

    void dividirHijo(Nodo *nodoPadre, int indice, Nodo *nodoHijo)
    {
        int medio = grado - 1;
        Nodo *nuevoNodo = new Nodo(grado, nodoHijo->hoja);
        nuevoNodo->n = grado - 1;

        for (int i = 0; i < grado - 1; i++)
        {
            nuevoNodo->claves[i] = nodoHijo->claves[i + grado];
        }

        if (!nodoHijo->hoja)
        {
            for (int i = 0; i < grado; i++)
            {
                nuevoNodo->punterosHijos[i] = nodoHijo->punterosHijos[i + grado];
            }
        }
        else
        {
            nuevoNodo->siguiente = nodoHijo->siguiente;
            nodoHijo->siguiente = nuevoNodo;
        }

        nodoHijo->n = grado;

        for (int i = nodoPadre->n; i > indice; i--)
        {
            nodoPadre->punterosHijos[i + 1] = nodoPadre->punterosHijos[i];
            nodoPadre->claves[i] = nodoPadre->claves[i - 1];
        }

        nodoPadre->punterosHijos[indice + 1] = nuevoNodo;
        nodoPadre->claves[indice] = nuevoNodo->claves[0];
        nodoPadre->n++;
    }

    void eliminar(int clave)
    {
        if (!raiz)
        {
            cout << "El árbol B+ está vacío" << endl;
            return;
        }

        eliminar(raiz, clave);

        if (raiz->n == 0)
        {
            Nodo *tmp = raiz;
            if (raiz->hoja)
            {
                raiz = nullptr;
            }
            else
            {
                raiz = raiz->punterosHijos[0];
            }
            delete tmp;
        }
    }

    void eliminar(Nodo *nodo, int clave)
    {
        int idx = encontrarClave(nodo, clave);

        if (nodo->hoja)
        {
            if (idx < nodo->n && nodo->claves[idx] == clave)
            {
                for (int i = idx; i < nodo->n - 1; i++)
                {
                    nodo->claves[i] = nodo->claves[i + 1];
                }
                nodo->n--;
            }
            else
            {
                cout << "La clave " << clave << " no existe\n";
                return;
            }
        }
        else
        {
            bool esUltimo = ((idx == nodo->n) ? true : false);
            if (nodo->punterosHijos[idx]->n < grado)
            {
                llenar(nodo, idx);
            }
            if (esUltimo && idx > nodo->n)
            {
                eliminar(nodo->punterosHijos[idx - 1], clave);
            }
            else
            {
                eliminar(nodo->punterosHijos[idx], clave);
            }
        }
    }

    int encontrarClave(Nodo *nodo, int clave)
    {
        int idx = 0;
        while (idx < nodo->n && nodo->claves[idx] < clave)
        {
            ++idx;
        }
        return idx;
    }

    void llenar(Nodo *nodo, int idx)
    {
        if (idx != 0 && nodo->punterosHijos[idx - 1]->n >= grado)
        {
            tomarPrestadoAnterior(nodo, idx);
        }
        else if (idx != nodo->n && nodo->punterosHijos[idx + 1]->n >= grado)
        {
            tomarPrestadoSiguiente(nodo, idx);
        }
        else
        {
            if (idx != nodo->n)
            {
                fusionar(nodo, idx);
            }
            else
            {
                fusionar(nodo, idx - 1);
            }
        }
    }

    void tomarPrestadoAnterior(Nodo *nodo, int idx)
    {
        Nodo *hijo = nodo->punterosHijos[idx];
        Nodo *hermano = nodo->punterosHijos[idx - 1];

        for (int i = hijo->n - 1; i >= 0; --i)
        {
            hijo->claves[i + 1] = hijo->claves[i];
        }

        if (!hijo->hoja)
        {
            for (int i = hijo->n; i >= 0; --i)
            {
                hijo->punterosHijos[i + 1] = hijo->punterosHijos[i];
            }
        }

        hijo->claves[0] = nodo->claves[idx - 1];

        if (!hijo->hoja)
        {
            hijo->punterosHijos[0] = hermano->punterosHijos[hermano->n];
        }

        nodo->claves[idx - 1] = hermano->claves[hermano->n - 1];

        hijo->n += 1;
        hermano->n -= 1;
    }

    void tomarPrestadoSiguiente(Nodo *nodo, int idx)
    {
        Nodo *hijo = nodo->punterosHijos[idx];
        Nodo *hermano = nodo->punterosHijos[idx + 1];

        hijo->claves[hijo->n] = nodo->claves[idx];

        if (!hijo->hoja)
        {
            hijo->punterosHijos[hijo->n + 1] = hermano->punterosHijos[0];
        }

        nodo->claves[idx] = hermano->claves[0];

        for (int i = 1; i < hermano->n; ++i)
        {
            hermano->claves[i - 1] = hermano->claves[i];
        }

        if (!hermano->hoja)
        {
            for (int i = 1; i <= hermano->n; ++i)
            {
                hermano->punterosHijos[i - 1] = hermano->punterosHijos[i];
            }
        }

        hijo->n += 1;
        hermano->n -= 1;
    }

    void fusionar(Nodo *nodo, int idx)
    {
        Nodo *hijo = nodo->punterosHijos[idx];
        Nodo *hermano = nodo->punterosHijos[idx + 1];

        if (!hijo->hoja)
        {
            hijo->claves[grado - 1] = nodo->claves[idx];

            for (int i = 0; i < hermano->n; ++i)
            {
                hijo->claves[i + grado] = hermano->claves[i];
            }

            for (int i = 0; i <= hermano->n; ++i)
            {
                hijo->punterosHijos[i + grado] = hermano->punterosHijos[i];
            }
        }
        else
        {
            for (int i = 0; i < hermano->n; ++i)
            {
                hijo->claves[i + grado - 1] = hermano->claves[i];
            }
            hijo->siguiente = hermano->siguiente;
        }

        for (int i = idx + 1; i < nodo->n; ++i)
        {
            nodo->claves[i - 1] = nodo->claves[i];
        }

        for (int i = idx + 2; i <= nodo->n; ++i)
        {
            nodo->punterosHijos[i - 1] = nodo->punterosHijos[i];
        }

        hijo->n += hermano->n + (hijo->hoja ? 0 : 1);
        nodo->n--;

        delete hermano;
    }

    void recorrer()
    {
        Nodo *actual = raiz;
        while (!actual->hoja)
        {
            actual = actual->punterosHijos[0];
        }

        while (actual)
        {
            for (int i = 0; i < actual->n; i++)
            {
                cout << actual->claves[i] << " ";
            }
            actual = actual->siguiente;
        }
        cout << endl;
    }
};

int main()
{
    BPlusTree BPT(3);

    BPT.insertar(10);
    BPT.insertar(20);
    BPT.insertar(5);
    BPT.insertar(6);
    BPT.insertar(12);
    BPT.insertar(30);
    BPT.insertar(7);
    BPT.insertar(17);

    cout << "Recorrido: ";
    BPT.recorrer();

    int clave = 6;
    if (BPT.buscar(clave) != nullptr)
    {
        cout << "Clave " << clave << " encontrada" << endl;
    }
    else
    {
        cout << "Clave " << clave << " no encontrada" << endl;
    }

    BPT.eliminar(6);
    cout << "Recorrido despues de eliminar 6: ";
    BPT.recorrer();

    return 0;
}
