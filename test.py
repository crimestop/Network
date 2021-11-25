import TAT
import net

lat = net.Network()
lat.add("A")
lat.add("B")
lat.add("C")
lat.add("D")
lat.set_edge("A", "B")
lat.set_edge("A", "C")
lat.set_edge("B", "C")
lat.set_edge("C", "D")
print(lat)
# lat.draw("test")

cjlat = net.double_tnenv(lat)
# cjlat.draw("cjtest")

lat2 = net.Network()
for i in range(4):
    for j in range(4):
        lat2.add(f"ten{i}_{j}")
        lat2[f"ten{i}_{j}"] = TAT.No.D.Tensor(1)
        # print(lat2[f"ten{i}_{j}"])
for i in range(4):
    for j in range(4):
        if i != 0:
            lat2.set_edge(f"ten{i-1}_{j}", f"ten{i}_{j}")
        if j != 0:
            lat2.set_edge(f"ten{i}_{j-1}", f"ten{i}_{j}")
# lat2.draw("square")


def init_node(names):
    result = TAT.No.D.Tensor(names, [2 for _ in names])
    gen = TAT.random.uniform_real(-1, +1)
    result.set(gen)
    return result


lat2.init_nodes(init_node)
lat2.absorb_no_absorb_contract("ten1_1", "ten1_2")
# lat2.draw("square")
tot = lat2.contract_no_absorb_contract()
print(tot)

tnt = net.Group()
tnt.belong(lat2)
for i in range(4):
    for j in range(4):
        if f"ten{i}_{j}" in lat2:
            tnt.absorb_no_absorb_contract(f"ten{i}_{j}")
