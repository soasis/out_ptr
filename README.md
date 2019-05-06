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
    <td>Shared Code</td>
    <td></td>
  </tr>
  <tr>
    <td>```<br>#include &lt;memory&gt;<br><br>#include &lt;avformat.h&gt;<br><br><br>struct AVFormatContextDeleter {<br><br>          void operator() (AVFormatContext* c) noexcept {<br><br>               avformat_close_input(&amp;c);<br><br>               avformat_free_context(c);<br><br>          }<br><br>};<br><br>typedef std::unique_ptr&lt;AVFormatContext, AVFormatContextDeleter&gt; AVFormatContext;<br><br>// Signature from libavformat:<br><br>// int avformat_open_input(AVFormatContext **ps, const char *url, AVInputFormat *fmt, AVDictionary **options);<br>```<br></td>
    <td></td>
  </tr>
  <tr>
    <td>Current Code</td>
    <td>With Proposal</td>
  </tr>
  <tr>
    <td>```<br>int main (int, char* argv[]) {<br><br>     AVFormatContext context(avformat_alloc_context());<br><br>     // ...<br><br>     // used, need to reopen<br><br>     AVFormatContext* raw_context = context.release();<br><br>     if (avformat_open_input(&amp;raw_context, <br><br>          argv[0], nullptr, nullptr) != 0) {<br><br>          std::stringstream ss;<br><br>          ss &lt;&lt; "ffmpeg_image_loader could not open file '"<br><br>               &lt;&lt; path &lt;&lt; "'";<br><br>          throw FFmpegInputException(ss.str().c_str());<br><br>     }<br><br>     context.reset(raw_context);<br><br><br>     // ... off to the races !<br><br><br>     return 0;<br><br>}<br>```<br></td>
    <td>```<br>int main (int, char* argv[]) {<br><br>     AVFormatContext context(avformat_alloc_context());<br><br>     // ...<br><br>     // used, need to reopen<br><br><br>     if (avformat_open_input(std::inout_ptr(context), <br><br>          argv[0], nullptr, nullptr) != 0) {<br><br>          std::stringstream ss;<br><br>          ss &lt;&lt; "ffmpeg_image_loader could not open file '"<br><br>               &lt;&lt; argv[0] &lt;&lt; "'";<br><br>          throw FFmpegInputException(ss.str().c_str());<br><br>     }<br><br><br><br>     // ... off to the races!<br><br><br>     return 0;<br><br>}<br>```<br></td>
  </tr>
</table>


# Extending for your own pointer types

Extension for Smart Pointers is done by using (Partial) Template Specialization on the types `out_ptr_t` and `inout_ptr_t` for your pointer types. Here is an example for a type called [sg14::retain_ptr](https://github.com/slurps-mad-rips/retain-ptr):

```

```


