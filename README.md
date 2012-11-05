# indicator-text

An indicator for the Unity panel that show arbitrary text. You can use it to
show system information, a fortune cookie or daily joke, or even as simple IM.

`indicator-text` listens for text input on various sources. It then simply
outputs the text to the panel. 

## Screenshots

Panel showing output from conky.

![indicator-text screenshot 1](https://github.com/eglimi/indicator-text/raw/master/img/indicator-text-1.png "indicator-text screenshot 1")

Panel showing arbitrary text from colleague.

![indicator-text screenshot 2](https://github.com/eglimi/indicator-text/raw/master/img/indicator-text-2.png "indicator-text screenshot 2")

## Motivation

After using [WindowMaker](http://windowmaker.org/) for many years, I tried the
Unity desktop. One thing I immediately missed were the dock apps. At least, I
wanted to show some system information such as current CPU load and memory
usage.

There exist some indicators for this purpose. However, I found that they do not
fit into the desktop aesthetically. This is why I wrote this indicator.

## Installation

### Source

	$ sudo apt-get install libgtk-3-dev libappindicator3-dev
	$ git clone https://github.com/eglimi/indicator-text.git
	$ cd indicator-text
	$ make
	$ ./indicator-text

### Autostart

Ubuntu uses `.desktop` files to manage autostart. You may use the one provided
with the sources.

	$ cp ./indicator-text /usr/local/bin
	$ cp ./indicator-text.desktop $HOME/.config/autostart

### Ubuntu Package

Currently, there is no pre-compiled package. I might be able to provide one, if
somebody is interested. Just write a feature request.

## Input channels

Currently, `indicator-text` supports the following input.

* `stdin`
* UDP messages on port 3434 (address is configurable)

More channels could be easily added if somebody is interested. Again, write a
feature request.

## Usage

### stdin

The most simple way is to use the `stdin` channel. For this, simply start the
program and type something. When you hit `ENTER`, you should see your text in
the panel.

This is already very useful because it means that text can be piped into the
program. Since this is a common pattern in Linux, many programs have the option
to have their output sent to `stdout`. If the program does this repeatedly,
this is all we need.

	$ repeating_prg | ./indicator-text

### UDP messages

This is the more flexible way, because it does make less assumptions about the
program generating the text. Using this channel, a program can send its text
either directly to the appropriate host and port, or through `stdout` and a
program such as [socat](http://www.dest-unreach.org/socat/) or
[netcat](http://netcat.sourceforge.net/).

Using this technique, there can be several programs sending text to the panel.
The general pattern is to start `indicator-text`, and sending text from other
sources independently.

	$ ./indicator-text
	$ echo "hello panel, it's me!" | socat - UDP:localhost:3434

One can simply start `socat` as shown above and start typing. This mimics the
first use case with `stdin`, but with the advantage that multiple sources can
send messages.

## Examples

### System Information

Perhaps the most useful usage is to show important system information.
`indicator-text` itself does not have any means to provide such information.
Instead, it can be used together with a system monitor such as
[conky](http://conky.sourceforge.net/).

Install conky

	$ sudo apt-get install conky-cli

Create a conky configuration that shows the your most important information in
one line. The example from the screenshot uses the following configuaration.
Copy it to `$HOME/.conkyrc`.

	background no
	out_to_console yes
	out_to_x no
	update_interval 2.0
	use_spacer | 
	
	TEXT
	cpu: ${cpu cpu0}% | mem: ${memperc}% | ↑: ${upspeedf}K ↓:${downspeedf}K

Start it with either

	$ conky | ./indicator-text

Or using UDP

	$ ./indicator-text
	$ conky | socat - UDP:localhost:3434

In order to autostart `conky` and `indicator-text`, you can create a script
with the following content and adjust `indicator-text.desktop` to reference
this script. Or alternatively, again, start conky and indicator-text separately
and use UDP.

	#!/bin/sh
	/usr/bin/conky | /usr/local/bin/indicator-text


### Fortune cookies

There are several fortune cookies available on your system. See a list an
select your favourite fortunes.

	$ apt-cache search fortunes
	$ sudo apt-get install fortunes

Start `indicator-text` and send the fortune text (a bit complicated because we
have only one line).
	
	$ ./indicator-text
	$ fortune -s | tr -d '\n' | sed -e 's/$/\n/' | socat - UDP:localhost:3434

## License

This program is distributed under the terms of the MIT license. See
[LICENSE](https://github.com/eglimi/indicator-text/raw/master/LICENSE) file.
