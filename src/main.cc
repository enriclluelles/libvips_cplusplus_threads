#include <cstdio>
#include <future>
#include <mutex>
#include <vector>
#include <iterator>
#include <vips/vips8>

using namespace vips;
using namespace std;

int
main (int argc, char **argv)
{
  if (VIPS_INIT (argv[0]))
    vips_error_exit (NULL);

  if (argc != 5)
    vips_error_exit ("usage: %s input-file output-file width height", argv[0]);

  auto source = VSource::new_from_file(argv[1]);
  auto first_image = VImage::new_from_source(source, "[n=1, page=0]");
  auto npages = first_image.get_int("n-pages");
  auto delay = first_image.get_array_int("delay");

  int width = atoi(argv[3]);
  int height = atoi(argv[4]);

  mutex m;

  vector<VImage> v(npages);
  vector<future<void>> f;

  for (int i = 0; i < npages; i++) {
    f.push_back(std::async([&v, i, width, height, source, &m] () {
      auto opts = "[n=1, page=" + to_string(i) + "]";
      auto page = VImage::new_from_source(source, opts.c_str());
      auto option = VImage::option()->set("height", height);

      auto result = VImage::new_memory();
      auto th = page
        .thumbnail_image(width, option)
        .bandjoin(255)
        .gravity(VipsCompassDirection::VIPS_COMPASS_DIRECTION_CENTRE, width, height)
        .write(result);

      m.lock();
      v[i] = result;
      m.unlock();
    }));
  }

  for (auto &future : f) {
    future.wait();
  }

  auto joined = VImage::arrayjoin(v, VImage::option()->set("across", 1));
  joined.set("page-height", height);
  joined.set("delay", std::vector(delay.begin(), delay.begin() + npages));
  joined.write_to_file(argv[2]);

  vips_shutdown ();

  return 0;
}
