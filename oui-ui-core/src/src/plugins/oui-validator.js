const pattern = {
  email: /^(([^<>()[\]\\.,;:\s@"]+(\.[^<>()[\]\\.,;:\s@"]+)*)|(".+"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$/,
  url: new RegExp('^(?!mailto:)(?:(?:http|https|ftp)://|//)(?:\\S+(?::\\S*)?@)?(?:(?:(?:[1-9]\\d?|1\\d\\d|2[01]\\d|22[0-3])(?:\\.(?:1?\\d{1,2}|2[0-4]\\d|25[0-5])){2}(?:\\.(?:[0-9]\\d?|1\\d\\d|2[0-4]\\d|25[0-4]))|(?:(?:[a-z\\u00a1-\\uffff0-9]+-*)*[a-z\\u00a1-\\uffff0-9]+)(?:\\.(?:[a-z\\u00a1-\\uffff0-9]+-*)*[a-z\\u00a1-\\uffff0-9]+)*(?:\\.(?:[a-z\\u00a1-\\uffff]{2,})))|localhost)(?::\\d{2,5})?(?:(/|\\?|#)[^\\s]*)?$', 'i')
}

function parseIPv4 (str) {
  if ((typeof (str) !== 'string' && !(str instanceof String)) ||
    !str.match(/^\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}$/)) { return undefined }

  const num = []
  const parts = str.split(/\./)

  for (let i = 0; i < parts.length; i++) {
    const n = parseInt(parts[i])
    if (isNaN(n) || n > 255) { return undefined }

    num.push(n)
  }

  return num
}

function parseIPv6 (str) {
  if ((typeof (str) !== 'string' && !(str instanceof String)) ||
    !str.match(/^[a-fA-F0-9:]+(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})?$/)) { return undefined }

  const parts = str.split(/::/)
  if (parts.length === 0 || parts.length > 2) { return undefined }

  const lnum = []
  if (parts[0].length > 0) {
    const left = parts[0].split(/:/)
    for (let i = 0; i < left.length; i++) {
      const n = parseInt(left[i], 16)
      if (isNaN(n)) { return undefined }

      lnum.push((n / 256) >> 0)
      lnum.push(n % 256)
    }
  }

  const rnum = []
  if (parts.length > 1 && parts[1].length > 0) {
    const right = parts[1].split(/:/)

    for (let i = 0; i < right.length; i++) {
      if (right[i].indexOf('.') > 0) {
        const addr = parseIPv4(right[i])
        if (!addr) { return undefined }

        rnum.push.apply(rnum, addr)
        continue
      }

      const n = parseInt(right[i], 16)
      if (isNaN(n)) { return undefined }

      rnum.push((n / 256) >> 0)
      rnum.push(n % 256)
    }
  }

  if (rnum.length > 0 && (lnum.length + rnum.length) > 15) { return undefined }

  const num = []

  num.push.apply(num, lnum)

  for (let i = 0; i < (16 - lnum.length - rnum.length); i++) { num.push(0) }

  num.push.apply(num, rnum)

  if (num.length > 16) { return undefined }

  return num
}

function isNetmask (addr) {
  if (!Array.isArray(addr)) { return false }

  let c

  for (c = 0; (c < addr.length) && (addr[c] === 255); c++);

  if (c === addr.length) { return true }

  if ((addr[c] === 254) || (addr[c] === 252) || (addr[c] === 248) ||
    (addr[c] === 240) || (addr[c] === 224) || (addr[c] === 192) ||
    (addr[c] === 128) || (addr[c] === 0)) {
    for (c++; (c < addr.length) && (addr[c] === 0); c++);

    if (c === addr.length) { return true }
  }

  return false
}

export default {
  ip4addr (value) {
    return !!parseIPv4(value)
  },
  ip6addr (value) {
    return !!parseIPv6(value)
  },
  ipaddr (value) {
    return !!parseIPv4(value) || !!parseIPv6(value)
  },
  netmask4 (value) {
    return isNetmask(parseIPv4(value))
  },
  netmask6 (value) {
    return isNetmask(parseIPv6(value))
  },
  macaddr (value) {
    return !!value.match(/^([a-fA-F0-9]{2}:){5}[a-fA-F0-9]{2}$/)
  },
  hostname (value) {
    return (value.length <= 253) && ((!!value.match(/^[a-zA-Z0-9]+$/) || (!!value.match(/^[a-zA-Z0-9_][a-zA-Z0-9_\-.]*[a-zA-Z0-9]$/) && !!value.match(/[^0-9.]/))))
  },
  host (value) {
    return this.hostname(value) || this.ipaddr(value)
  },
  url (value) {
    return !!value.match(pattern.url)
  },
  email (value) {
    return !!value.match(pattern.email)
  },
  number (value) {
    return !isNaN(value)
  },
  integer (value) {
    return this.number(value) && /(^[0-9]\d*$)/.test(Number(value) + '')
  },
  uinteger (value) {
    return this.integer(value) && value >= 0
  },
  port (value) {
    return this.uinteger(value) && value <= 65535
  },
  uci (value) {
    return !!value.match(/^[a-zA-Z0-9_]+$/)
  }
}
