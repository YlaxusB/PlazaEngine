/*
//#include "Engine/Vendor/yaml/include/yaml-cpp/contrib/graphbuilder.h"
#include "graphbuilderadapter.h"

#include "yaml-cpp/parser.h"  // IWYU pragma: keep

namespace YAML {
class GraphBuilderInterface;

void* BuildGraphOfNextDocument(Parser& parser,
                               GraphBuilderInterface& graphBuilder) {
  GraphBuilderAdapter eventHandler(graphBuilder);
  if (parser.HandleNextDocument(eventHandler)) {
    return eventHandler.RootNode();
  }
  return nullptr;
}
}  // namespace YAML
*/