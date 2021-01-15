#include "graphic/vk_render/renderGraph/RenderGraph.h"
#include "stl/CreviceSTL.h"

using namespace crevice;

void test1() {
  auto rg = RenderGraph();

  auto p1 = make_shared<RenderPass>(RenderPass());
  auto p2 = make_shared<RenderPass>(RenderPass());
  auto p3 = make_shared<RenderPass>(RenderPass());

  auto p1Id = rg.addPass(p1);
  auto p2Id = rg.addPass(p2);
  auto p3Id = rg.addPass(p3);

  rg.linkNode(p1->id, p2->id);
  rg.linkNode(p2->id, p3->id);
  rg.setOutputPass(p3->id);

  rg.analyzeExecutionOrder();

  std::cout <<"order:";
  for (auto i : rg.mExeOrder) {
    std::cout << i << ",";
  }
  std::cout << std::endl;
  std::cout <<"start:";
  for (auto i : rg.startNodes) {
    std::cout << i << ",";
  }
  std::cout << std::endl;
  std::cout << "test1 good" << std::endl;
}

void test2() {
  auto rg = RenderGraph();

  auto p1 = make_shared<RenderPass>(RenderPass());
  auto p2 = make_shared<RenderPass>(RenderPass());
  auto p3 = make_shared<RenderPass>(RenderPass());
  auto p4 = make_shared<RenderPass>(RenderPass());
  auto p5 = make_shared<RenderPass>(RenderPass());
  auto p6 = make_shared<RenderPass>(RenderPass());
  auto p7 = make_shared<RenderPass>(RenderPass());
  auto p8 = make_shared<RenderPass>(RenderPass());
  auto p9 = make_shared<RenderPass>(RenderPass());
  auto p10 = make_shared<RenderPass>(RenderPass());
  auto p11 = make_shared<RenderPass>(RenderPass());
  auto p12 = make_shared<RenderPass>(RenderPass());
  auto p13 = make_shared<RenderPass>(RenderPass());
  auto p14 = make_shared<RenderPass>(RenderPass());
  auto p15 = make_shared<RenderPass>(RenderPass());
  auto p16 = make_shared<RenderPass>(RenderPass());

  rg.addPass(p1);
  rg.addPass(p2);
  rg.addPass(p3);
  rg.addPass(p4);
  rg.addPass(p5);
  rg.addPass(p6);
  rg.addPass(p7);
  rg.addPass(p8);
  rg.addPass(p9);
  rg.addPass(p10);
  rg.addPass(p11);
  rg.addPass(p12);
  rg.addPass(p13);
  rg.addPass(p14);
  rg.addPass(p15);
  rg.addPass(p16);

  rg.linkNode(p1->id, p2->id);
  rg.linkNode(p1->id, p16->id);
  rg.linkNode(p10->id, p8->id);
  rg.linkNode(p15->id, p8->id);
  rg.linkNode(p8->id, p2->id);
  rg.linkNode(p8->id, p12->id);
  rg.linkNode(p8->id, p16->id);
  rg.linkNode(p2->id, p3->id);
  rg.linkNode(p12->id, p14->id);
  rg.linkNode(p12->id, p13->id);
  rg.linkNode(p16->id, p3->id);
  rg.linkNode(p14->id, p3->id);
  rg.linkNode(p3->id, p4->id);
  rg.linkNode(p3->id, p5->id);

  rg.linkNode(p6->id, p7->id);
  rg.linkNode(p6->id, p11->id);

  rg.setOutputPass(p4->id);

  //   rg.setOutputPass(p7->id);

  rg.analyzeExecutionOrder();

  std::cout <<"order:";
  for (auto i : rg.mExeOrder) {
    std::cout << i << ",";
  }
  std::cout << std::endl;
  std::cout <<"start:";
  for (auto i : rg.startNodes) {
    std::cout << i << ",";
  }
  std::cout << std::endl;
  std::cout << "test2 good" << std::endl;
}

void test3() {
  auto rg = RenderGraph();

  auto p1 = make_shared<RenderPass>(RenderPass());
  auto p2 = make_shared<RenderPass>(RenderPass());
  auto p3 = make_shared<RenderPass>(RenderPass());
  auto p4 = make_shared<RenderPass>(RenderPass());
  auto p5 = make_shared<RenderPass>(RenderPass());
  auto p6 = make_shared<RenderPass>(RenderPass());
  auto p7 = make_shared<RenderPass>(RenderPass());
  auto p8 = make_shared<RenderPass>(RenderPass());
  auto p9 = make_shared<RenderPass>(RenderPass());
  auto p10 = make_shared<RenderPass>(RenderPass());
  auto p11 = make_shared<RenderPass>(RenderPass());
  auto p12 = make_shared<RenderPass>(RenderPass());
  auto p13 = make_shared<RenderPass>(RenderPass());
  auto p14 = make_shared<RenderPass>(RenderPass());
  auto p15 = make_shared<RenderPass>(RenderPass());
  auto p16 = make_shared<RenderPass>(RenderPass());

  rg.addPass(p1);
  rg.addPass(p2);
  rg.addPass(p3);
  rg.addPass(p4);
  rg.addPass(p5);
  rg.addPass(p6);
  rg.addPass(p7);
  rg.addPass(p8);
  rg.addPass(p9);
  rg.addPass(p10);
  rg.addPass(p11);
  rg.addPass(p12);
  rg.addPass(p13);
  rg.addPass(p14);
  rg.addPass(p15);
  rg.addPass(p16);

  rg.linkNode(p1->id, p2->id);
  rg.linkNode(p1->id, p16->id);
  rg.linkNode(p10->id, p8->id);
  rg.linkNode(p15->id, p8->id);
  rg.linkNode(p8->id, p2->id);
  rg.linkNode(p8->id, p12->id);
  rg.linkNode(p8->id, p16->id);
  rg.linkNode(p2->id, p3->id);
  rg.linkNode(p12->id, p14->id);
  rg.linkNode(p12->id, p13->id);
  rg.linkNode(p16->id, p3->id);
  rg.linkNode(p14->id, p3->id);
  rg.linkNode(p3->id, p4->id);
  rg.linkNode(p3->id, p5->id);

  rg.linkNode(p6->id, p7->id);
  rg.linkNode(p6->id, p11->id);

  // cycle
  rg.linkNode(p16->id, p15->id);

  rg.setOutputPass(p4->id);

  try {
    /* code */
    rg.analyzeExecutionOrder();
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    return;
  }

  std::cout <<"order:";
  for (auto i : rg.mExeOrder) {
    std::cout << i << ",";
  }
  std::cout << std::endl;
  std::cout << "test2 good" << std::endl;
}

int main(int argc, char const* argv[]) {
  test1();
  test2();
  test3();

  return 0;
}
