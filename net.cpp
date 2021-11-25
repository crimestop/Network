#include <TAT/TAT.hpp>
#include <net/net.hpp>
#include <net/tensor_network.hpp>
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using Network = net::tensor::TensorNetworkEnv<double>;
using Tensor = TAT::Tensor<double, TAT::NoSymmetry, std::string>;
using Group = net::group<Tensor, Tensor>;

namespace pybind11 {
	namespace detail {
		template <>
		struct type_caster<Tensor> {
			// set things up and gives you a `Tensor value;` member
			PYBIND11_TYPE_CASTER(Tensor, _("Tensor"));

			bool load(handle src, bool) {
				if (!src)
					return false;
				auto data = src.attr("dump")();
				value.load(py::cast<std::string>(data));
				return true;
			}

			static handle cast(Tensor v, return_value_policy /*policy*/, handle /*parent*/) {
				std::string data = v.dump();
				py::object tv_py = py::module::import("TAT").attr("No").attr("D").attr("Tensor")();
				tv_py.attr("load")(py::bytes(data));
				return tv_py.release();
			}
		};
	} // namespace detail
} // namespace pybind11

PYBIND11_MODULE(net, m) {
	m.doc() = "a tensor network library";

	py::class_<Network>(m, "Network")
			.def(py::init<>())
			.def(
					"add", [](Network & n, const std::string & name) { n.add(name); }, "Add a site into network")
			.def(
					"set_edge",
					[](Network & n, const std::string & name_1, const std::string & name_2) { n.set_edge(name_1, name_2); },
					"Add an edge into network")
			.def("__str__",
				  [](const Network & n) {
					  auto f = std::stringstream();
					  f << n;
					  return f.str();
				  })
			.def("__getitem__", [](Network & n, const std::string & name) -> Tensor { return n[name].val; })
			.def("__setitem__", [](Network & n, const std::string & name, const Tensor & t) { n[name].val = t; })
			.def("__contains__", [](Network & n, const std::string & name) -> bool { return n.contains(name); })
			.def("draw", [](Network & n, const std::string & name) { n.draw(name, true); })
			.def("init_nodes", [](Network & n, std::function<Tensor(const std::vector<std::string> &)> func) { n.init_nodes(func); })
			.def("absorb_no_absorb_contract",
				  [](Network & n, const std::string & name_1, const std::string & name_2) {
					  n.absorb(name_1, name_2, net::no_absorb(), net::tensor::contract());
				  })
			.def("contract_no_absorb_contract", [](Network & n) { return n.contract(net::no_absorb(), net::tensor::contract()); });

	py::class_<Group>(m, "Group")
			.def(py::init<>())
			.def(
					"belong", [](Group & g, Network & n) { g.belong(n); }, py::keep_alive<1, 2>())
			.def("absorb_no_absorb_contract", [](Group & g, const std::string & name) { g.absorb(name, net::no_absorb(), net::tensor::contract()); });

	m.def("double_tnenv", [](const Network & n) { return net::tensor::double_tnenv(n); });
}
