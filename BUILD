# TODO(vlasrado): describe this package.
package(default_visibility = ["//visibility:public"])
cc_binary (
    name = "monitoring_example",
    srcs = ["monitoring_example.cc"],
    deps = [
        "@google_cloud_cpp//:monitoring",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",

    ],
)

cc_binary (
    name = "logging_example",
    srcs = ["logging_example.cc"],
    deps = [
        "@google_cloud_cpp//:logging",
        "@com_google_absl//absl/strings",
    ],
)