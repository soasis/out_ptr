# boost.out_ptr

`boost.out_ptr` is a simple parameter wrapper for output pointers.

| Appveyor | Travis CI |
| -------- | --------- |
|          |           |
|          |           |
|          |           |

# Quick Example

This example comes from real world code using [libavformat]().

<table>
  <tr>
    <td colspan="2">Shared Code</td>
  </tr>
  <tr>
    <td colspan="2"><pre lang="cpp">
#include <memory>
#include <avformat.h>
// Signature from libavformat:
// int avformat_open_input(AVFormatContext **ps, const char *url, 
//	AVInputFormat *fmt, AVDictionary **options);

struct AVFormatContextDeleter {
		void operator() (AVFormatContext* c) noexcept {
			avformat_close_input(&c);
			avformat_free_context(c);
		}
};

typedef std::unique_ptr<AVFormatContext, AVFormatContextDeleter> AVFormatContext;
    </pre><br></td>
  </tr>
  <tr>
    <td>Current Code</td>
    <td>With boost.out_ptr !</td>
  </tr>
  <tr>
    <td><pre lang="cpp">
int main (int, char* argv[]) {
	AVFormatContext context(avformat_alloc_context());
	// ...
	// used, need to reopen
	AVFormatContext* raw_context = context.release();
	if (avformat_open_input(&raw_context, 
		argv[0], nullptr, nullptr) != 0) {
		std::stringstream ss;
		ss << "ffmpeg_image_loader could not open file '"
			<< path << "'";
		throw FFmpegInputException(ss.str().c_str());
	}
	context.reset(raw_context);

	// ... off to the races !

	return 0;
}
    </pre><br></td>
    <td><pre lang="cpp">
int main (int, char* argv[]) {
	AVFormatContext context(avformat_alloc_context());
	// ...
	// used, need to reopen
	AVFormatContext* raw_context = context.release();
	if (avformat_open_input(&raw_context, 
		argv[0], nullptr, nullptr) != 0) {
		std::stringstream ss;
		ss << "ffmpeg_image_loader could not open file '"
			<< path << "'";
		throw FFmpegInputException(ss.str().c_str());
	}
	context.reset(raw_context);

	// ... off to the races !

	return 0;
}
    </pre><br></td>
  </tr>
</table>


# Extending for your own pointer types

Extension for Smart Pointers is done by using (Partial) Template Specialization on the types `out_ptr_t` and `inout_ptr_t` for your pointer types. Here is an example for a type called [sg14::retain_ptr](https://github.com/slurps-mad-rips/retain-ptr):

```

```


