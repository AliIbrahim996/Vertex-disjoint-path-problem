from conans import ConanFile

class VDP_Workspace(ConanFile):
    name = "VDP_Workspace"
    generators = "virtualenv"

    def build_requirements(self):
        self.build_requires("cmake/3.23.2")
        self.build_requires("doxygen/1.9.4")
        self.build_requires("ninja/1.11.0")