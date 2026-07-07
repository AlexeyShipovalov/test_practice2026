from conan import ConanFile

class DictaphoneSTTConan(ConanFile):
    settings = "os", "compiler", "arch", "build_type"
    generators = "PkgConfigDeps"
    default_options = {
        "date/*:header_only": True,
        "date/*:use_system_tz_db": True
    }
    requires = (
        "kissfft/131.1.0@aurora",
        "onnxruntime/1.18.1@aurora",
    )
