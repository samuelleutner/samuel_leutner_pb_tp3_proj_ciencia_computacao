class Node:
    def __init__(self, chave, valor):
        self.chave = chave.lower()
        self.valor = valor
        self.esquerda = None
        self.direita = None
        self.altura = 1

class DicionarioAVL:
    def __init__(self):
        self.raiz = None
        self.total_itens = 0

    def obter_altura(self, no):
        if not no:
            return 0
        return no.altura

    def obter_balanceamento(self, no):
        if not no:
            return 0
        return self.obter_altura(no.esquerda) - self.obter_altura(no.direita)

    def rotacionar_direita(self, y):
        x = y.esquerda
        T2 = x.direita
        x.direita = y
        y.esquerda = T2
        y.altura = 1 + max(self.obter_altura(y.esquerda), self.obter_altura(y.direita))
        x.altura = 1 + max(self.obter_altura(x.esquerda), self.obter_altura(x.direita))
        return x

    def rotacionar_esquerda(self, x):
        y = x.direita
        T2 = y.esquerda
        y.esquerda = x
        x.direita = T2
        x.altura = 1 + max(self.obter_altura(x.esquerda), self.obter_altura(x.direita))
        y.altura = 1 + max(self.obter_altura(y.esquerda), self.obter_altura(y.direita))
        return y

    def inserir(self, chave, valor):
        self.raiz = self._inserir(self.raiz, chave, valor)

    def _inserir(self, no, chave, valor):
        if not no:
            self.total_itens += 1
            return Node(chave, valor)

        chave_lower = chave.lower()
        if chave_lower < no.chave:
            no.esquerda = self._inserir(no.esquerda, chave, valor)
        elif chave_lower > no.chave:
            no.direita = self._inserir(no.direita, chave, valor)
        else:
            no.valor = valor # Atualiza se a palavra já existe
            return no

        no.altura = 1 + max(self.obter_altura(no.esquerda), self.obter_altura(no.direita))
        balanceamento = self.obter_balanceamento(no)

        if balanceamento > 1 and chave_lower < no.esquerda.chave:
            return self.rotacionar_direita(no)
        if balanceamento < -1 and chave_lower > no.direita.chave:
            return self.rotacionar_esquerda(no)
        if balanceamento > 1 and chave_lower > no.esquerda.chave:
            no.esquerda = self.rotacionar_esquerda(no.esquerda)
            return self.rotacionar_direita(no)
        if balanceamento < -1 and chave_lower < no.direita.chave:
            no.direita = self.rotacionar_direita(no.direita)
            return self.rotacionar_esquerda(no)

        return no

    def buscar(self, chave):
        return self._buscar(self.raiz, chave.lower())

    def _buscar(self, no, chave):
        if not no:
            return None
        if chave == no.chave:
            return no.valor
        if chave < no.chave:
            return self._buscar(no.esquerda, chave)
        return self._buscar(no.direita, chave)

    def listar_palavras(self):
        print("--- Dicionário ---")
        self._listar_in_order(self.raiz)

    def _listar_in_order(self, no):
        if no:
            self._listar_in_order(no.esquerda)
            print(f"{no.chave}: {no.valor}")
            self._listar_in_order(no.direita)

    def remover(self, chave):
        self.raiz = self._remover(self.raiz, chave.lower())

    def _minimo_valor_no(self, no):
        atual = no
        while atual.esquerda is not None:
            atual = atual.esquerda
        return atual

    def _remover(self, no, chave):
        if not no:
            return no
        if chave < no.chave:
            no.esquerda = self._remover(no.esquerda, chave)
        elif chave > no.chave:
            no.direita = self._remover(no.direita, chave)
        else:
            if no.esquerda is None:
                temp = no.direita
                no = None
                self.total_itens -= 1
                return temp
            elif no.direita is None:
                temp = no.esquerda
                no = None
                self.total_itens -= 1
                return temp
            temp = self._minimo_valor_no(no.direita)
            no.chave = temp.chave
            no.valor = temp.valor
            no.direita = self._remover(no.direita, temp.chave)

        if no is None:
            return no

        no.altura = 1 + max(self.obter_altura(no.esquerda), self.obter_altura(no.direita))
        balanceamento = self.obter_balanceamento(no)

        if balanceamento > 1 and self.obter_balanceamento(no.esquerda) >= 0:
            return self.rotacionar_direita(no)
        if balanceamento < -1 and self.obter_balanceamento(no.direita) <= 0:
            return self.rotacionar_esquerda(no)
        if balanceamento > 1 and self.obter_balanceamento(no.esquerda) < 0:
            no.esquerda = self.rotacionar_esquerda(no.esquerda)
            return self.rotacionar_direita(no)
        if balanceamento < -1 and self.obter_balanceamento(no.direita) > 0:
            no.direita = self.rotacionar_direita(no.direita)
            return self.rotacionar_esquerda(no)

        return no

    def informar_altura(self):
        return self.obter_altura(self.raiz)

    def informar_total_itens(self):
        return self.total_itens

if __name__ == "__main__":
    dic = DicionarioAVL()
    print("Inserindo termos técnicos no dicionário...")
    dic.inserir("Processador", "Unidade central de processamento de dados.")
    dic.inserir("Memoria", "Armazenamento volátil de acesso rápido.")
    dic.inserir("Kernel", "Núcleo do sistema operacional.")
    dic.inserir("Cache", "Memória ultrarrápida embutida no processador.")

    print("\n")
    dic.listar_palavras()  

    print(f"\n[Busca] Significado de 'Kernel': {dic.buscar('Kernel')}")
    print(f"[Status] Total de Verbetes: {dic.informar_total_itens()}")
    print(f"[Status] Altura da Árvore AVL: {dic.informar_altura()}")
