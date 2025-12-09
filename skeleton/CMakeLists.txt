set(LLVM_LINK_COMPONENTS
  Core
  Support
)

add_llvm_pass_plugin(SkeletonPass
    # List your source files here.
    Skeleton.cpp
)
