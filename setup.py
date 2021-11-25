from glob import glob
from setuptools import setup
from pybind11.setup_helpers import Pybind11Extension

ext_modules = [
    Pybind11Extension(
        "net",
        sorted(glob("*.cpp")),
        include_dirs=[
            "include",
            "third_party/TAT/include",
            "third_party/kahypar",
            "third_party/kahypar/include",
            "third_party/kahypar/build",
            "third_party/kahypar/external_tools/googletest/googletest/include",
            "third_party/kahypar/external_tools",
        ],
        define_macros=[
            ("NET_USE_LIB_KAHYPAR", ""),
            ("NET_GRAPH_VIZ", ""),
            ("NET_SHOW_FIG_USE_GWENVIEW", ""),
            ("KAHYPAR_ENABLE_HEAVY_DATA_STRUCTURE_ASSERTIONS", ""),
            ("KAHYPAR_ENABLE_HEAVY_PREPROCESSING_ASSERTIONS", ""),
            ("KAHYPAR_ENABLE_HEAVY_COARSENING_ASSERTIONS", ""),
            ("KAHYPAR_ENABLE_HEAVY_INITIAL_PARTITIONING_ASSERTIONS", ""),
            ("KAHYPAR_ENABLE_HEAVY_REFINEMENT_ASSERTIONS", ""),
        ],
        library_dirs=[
            "third_party/kahypar/build/lib",
        ],
        libraries=[
            "boost_program_options",
            "lapack",
            "blas",
            "kahypar",
            "gvc",
            "cgraph",
        ],
    )
]

setup(
    name="net",
    ext_modules=ext_modules,
)
