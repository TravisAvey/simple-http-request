# Simple HTTP Request

## Description
This is a simple HTTP request library that will perform simple HTTP requests:

```
GET, POST, PUT, PATCH, DELETE
```

### Motivation
I decided to make this library, OK it's nothing more than a wrapper around libCURL, anyways... I made this because I have other project ideas in mind that would make use of libCURL but wanted a simplier interface.

## Installation
```
mkdir build
cd build
cmake -DTARGET_GROUP:STRING={release,test,examples} ..
make
```

You can also pass -DVERBOSE=on for debugging purposes (it will print a lot of information).

## Usage
Either clone this repo or download a release.


## Authors and acknowledgment
I'd like to thank myself for contributing

## License
MIT License
