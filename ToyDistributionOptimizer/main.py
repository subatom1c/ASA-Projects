import pulp
import tempfile

def extrai_stock_farbica(fabricas, id_fabrica):
    for i in fabricas:
        if i[0] == id_fabrica:
            return i[2]
    return -1

def imprimir_pedidos_atendidos(pedidos, x):
    """
    Imprime os pedidos que foram atendidos, ou seja, aqueles que têm pelo menos uma fábrica que atende a demanda.
    """
    print("Pedidos atendidos:")
    for k, _, fabricas_ck in pedidos:
        for f in fabricas_ck:
            if pulp.value(x[k, f]) == 1:
                print(f"Pedido {k} atendido pela fábrica {f}")
                break

def resolver_distribuicao():
    try:
        # Ler os parâmetros iniciais do usuário
        n, m, t = map(int, input().strip().split())

        # Verificar se os valores são válidos
        if n <= 0 or m <= 0 or t <= 0:
            return -1

        # Ler os dados das fábricas
        fabricas = []
        for _ in range(n):
            fi, pj, fmax = map(int, input().strip().split())
            fabricas.append((fi, pj, fmax))

        # Ler os dados dos países
        paises = []
        for _ in range(m):
            pj, pmaxj, pminj = map(int, input().strip().split())
            paises.append((pj, pmaxj, pminj))

        # Ler os pedidos das crianças
        pedidos = []
        for _ in range(t):
            dados = list(map(int, input().strip().split()))
            ck, pj, *fabricas_ck = dados
            fabricas_validas = [id_fabrica for id_fabrica in fabricas_ck if any(f[0] == id_fabrica for f in fabricas)]
            if fabricas_validas:
                for id_fabrica in fabricas_ck:
                    stock = extrai_stock_farbica(fabricas, id_fabrica)
                    if (stock > 0):
                        pedidos.append((ck, pj, fabricas_ck))
                        break

        # Filtrar pedidos que não podem ser atendidos por nenhuma fábrica
        if len(pedidos) == 0:
            return 0

        # Create the optimization problem
        problema = pulp.LpProblem("Distribuicao_de_Brinquedos", pulp.LpMaximize)

        # Decision variables: x[k, f] indicates if request k is fulfilled by factory f (1) or not (0)
        x = {(k, f): pulp.LpVariable(f"x_{k}_{f}", cat="Binary") for k, _, fabricas_ck in pedidos for f in fabricas_ck}

        # Objective function: maximize the number of requests fulfilled
        problema += pulp.lpSum(x[k, f] for k, _, fabricas_ck in pedidos for f in fabricas_ck), "Maximizar_pedidos_atendidos"
    
        # Factory capacity constraints
        for fi, _, fmax in fabricas:
            problema += (
                pulp.lpSum(
                    x[k, f] 
                    for k, _, fabricas_ck in pedidos 
                    for f in fabricas_ck if f == fi
                ) <= fmax,
                f"Capacidade_fabrica_{fi}"
            )

        # Export and import constraints per country
        for pj, pmaxj, pminj in paises: # Escolher um país
            # Maximum export constraint (excluding intra-country distribution)
            problema += (
                pulp.lpSum(
                    x[k, f] 
                    for k, pais, fabricas_ck in pedidos 
                    for f in fabricas_ck
                    if pais != pj and any(fi == f and pais_fabrica == pj for fi, pais_fabrica, _ in fabricas)
                ) <= pmaxj,
                f"Exportacao_maxima_{pj}"
            )
            # Minimum import constraint
            problema += (
                pulp.lpSum(
                    x[k, f] 
                    for k, pais, fabricas_ck in pedidos 
                    for f in fabricas_ck if pais == pj
                ) >= pminj,
                f"Exportacao_minima_{pj}"
            )
        # 
        # Each request can only be fulfilled by one factory
        for k, _, fabricas_ck in pedidos:
            problema += (
                pulp.lpSum(x[k, f] for f in fabricas_ck) <= 1,
                f"Pedido_{k}_unico"
            )
        
        # Resolver o problema
        try:
            status = problema.solve(pulp.PULP_CBC_CMD(msg=0))
        except Exception as e:
            return -1

        # Verificar o status do solver
        if pulp.LpStatus[status] not in ["Optimal", "Feasible"]:
            return -1

        # Calcular o número de pedidos atendidos
        try:
            resultado = sum(
                1 for k, _, fabricas_ck in pedidos
                if any(pulp.value(x[k, f]) == 1 for f in fabricas_ck)
            )
        except Exception as e:
            return -1

        

        return resultado

    except Exception as e:
        return -1

# Exemplo de uso
print(resolver_distribuicao())