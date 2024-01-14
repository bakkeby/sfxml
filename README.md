# `sfxml`: A simple XML formatter

`sfxml` is a simple tool that reads XML-like data from stdin, formats and indents it and writes
the result to standard out.

Contrary to generally recommended tools like `xmllint --format` this program does not perform any
form of validation to ensure that the input is valid XML.

Instead formatting is done on a best effort basis following basic rules.

The benefit of this is that the output will be consistent even for incomplete or invalid data and
the tool will not refuse to do the job if it doesn't understand it.

This is in comparison to many beautifiers and pretty-printing solutions that fall over because there
is a closing tag missing at the end.

This is a tool for when you have a condensed (i.e. a single line) of XML that is several pages long
and you just need to make parts of it more readable while investigating something.

It works by adding line breaks and indentation after opening and closing tags while ignoring escaped
characters as well as leaving text encapsulated in double or single quotes as-is.

## Installation

```
$ git clone https://github.com/bakkeby/sfxml.git
$ cd sfxml
$ sudo make install
$ sfxml
```

## License

`sfxml` is available under an MIT license. See the `LICENSE` file.
