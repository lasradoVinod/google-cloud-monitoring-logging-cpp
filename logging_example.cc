#include "google/cloud/logging/logging_service_v2_client.h"
#include "google/cloud/project.h"
#include "google/cloud/common_options.h"
#include "google/cloud/credentials.h"
#include "google/protobuf/util/time_util.h"

#include "absl/strings/str_cat.h"
#include "absl/strings/substitute.h"
#include "absl/strings/str_format.h"

#include <cstdlib>
#include <fstream>
#include <unistd.h>
#include <iostream>

namespace logging = ::google::cloud::logging;
constexpr char kCloudLoggingPathTemplate[] = "projects/$0/logs/";

google::api::MonitoredResource CreateMontioredResource(const std::string& project_id){
  char hostname[HOST_NAME_MAX];
  gethostname(hostname, HOST_NAME_MAX);
  google::api::MonitoredResource resource;
  auto labels = *resource.mutable_labels();
  resource.set_type("generic_task");
  labels["project_id"] = project_id;
  labels["job"] = "test";
  labels["task_id"] = hostname;
  return resource;
}

google::logging::v2::LogEntry MakeLogEntry(const std::string& project_id,
                                                            uint32_t random) {
  auto log_entry = google::logging::v2::LogEntry();
  *log_entry.mutable_resource() = CreateMontioredResource(project_id);

  log_entry.set_severity(google::logging::type::LogSeverity::INFO);
 
  log_entry.set_text_payload(absl::StrFormat("{\"test\":%u",random));
  return log_entry;
}


int main(int argc, char* argv[]) try {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " project-id  <path-to-service-creds>\n";
    return 1;
  }


  auto is = std::ifstream(argv[2]);
  
  auto contents = std::string(std::istreambuf_iterator<char>(is.rdbuf()), {});
  auto options =
  google::cloud::Options{}.set<google::cloud::UnifiedCredentialsOption>(
            google::cloud::MakeServiceAccountCredentials(contents));
  
  auto client = logging::LoggingServiceV2Client(
      logging::MakeLoggingServiceV2Connection(options));
  auto const project = google::cloud::Project(argv[1]);


  std::vector <google::logging::v2::LogEntry> log_entries;

  for (int i=0;i<10;i++){
    log_entries.emplace_back(MakeLogEntry(project.project_id(),rand()));
  }

  std::map<std::string, std::string> labels = {{"vinod","lasrado"}};

  auto response = client.WriteLogEntries(absl::StrCat(absl::Substitute(kCloudLoggingPathTemplate, project.project_id()), "test"),
                        CreateMontioredResource(project.project_id()),
                        labels,
                        log_entries);
  if (!response.ok()){
    std::cout << response.status() << std::endl;
  }
  return 0;
} catch (google::cloud::Status const& status) {
  std::cerr << "google::cloud::Status thrown: " << status << "\n";
  return 1;
}
