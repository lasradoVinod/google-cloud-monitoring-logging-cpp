#include "google/cloud/monitoring/metric_client.h"
#include "google/cloud/project.h"
#include <iostream>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/substitute.h"
#include "absl/strings/str_format.h"
#include "absl/time/time.h"


#include <cstdlib>

constexpr char kMetricTypePrefix[] = "custom.googleapis.com";
namespace monitoring = ::google::cloud::monitoring;

absl::StatusOr<google::api::MetricDescriptor> 
    CreateMetricDesciptor (monitoring::MetricServiceClient & client,
                                   std::string project_id){
  google::monitoring::v3::CreateMetricDescriptorRequest request;
  request.set_name(project_id);

  google::api::MetricDescriptor* metric_descriptor = request.mutable_metric_descriptor();

  metric_descriptor->set_type(absl::StrCat(kMetricTypePrefix, "/test"));
  metric_descriptor->set_description("Testing metric creation");
  metric_descriptor->clear_labels();
  auto labels = metric_descriptor->add_labels();
  labels->set_key("testing");
  labels->set_value_type(google::api::LabelDescriptor::STRING);
  labels->set_key("vinod");
  labels->set_description("");
  metric_descriptor->set_metric_kind(google::api::MetricDescriptor::GAUGE);
  metric_descriptor->set_value_type(google::api::MetricDescriptor::INT64);

  auto response = client.CreateMetricDescriptor(request);
  if (!response.ok()){
    std::cout << response.status();
    return absl::InternalError("Cannot create descriptor");
  
  }

  return *response;
}


int main(int argc, char* argv[]) try {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " project-id\n";
    return 1;
  }

  monitoring::MetricServiceClient client = monitoring::MetricServiceClient(
      monitoring::MakeMetricServiceConnection());

  std::cout << "Created connection" << std::endl;
  auto const project = google::cloud::Project(argv[1]);

  auto response = CreateMetricDesciptor(client,project.FullName());
  if (!response.ok()){
    std::cout << "error createing metric descriptor" << std::endl;
    return 0;
  }

  for (int i=0;i<10;i++){
    google::monitoring::v3::CreateTimeSeriesRequest request;
    request.set_name(project.FullName());

    auto time_series = request.add_time_series();
    time_series->mutable_metric()->set_type(absl::StrCat(kMetricTypePrefix, "/test"));
    time_series->mutable_resource()->set_type("global");
    auto point = time_series->add_points();
    point->mutable_value()->set_int64_value(rand());
    auto timestamp = point->mutable_interval()->mutable_end_time();
    absl::Time t1 = absl::Now();
    const int64_t sec = absl::ToUnixSeconds(t1);
    timestamp->set_seconds(sec);
    timestamp->set_nanos((t1 - absl::FromUnixSeconds(sec)) / absl::Nanoseconds(1));
    sleep(6);

    if (request.time_series().empty()) {
      std::cerr << "Could not add time series" << std::endl;
      return 0;
    }

    auto status = client.CreateTimeSeries(request);
    if (!status.ok()){
      std::cerr << "sending time series error " << status << std::endl;
      return 0;
    }
  }


  return 0;
} catch (google::cloud::Status const& status) {
  std::cerr << "google::cloud::Status thrown: " << status << "\n";
  return 1;
}