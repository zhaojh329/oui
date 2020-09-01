function formatUnit (param, base, pPrecision) {
  const pr = pPrecision ? Math.floor(10 * parseFloat('0' + pPrecision)) : 2
  const units = ['', 'K', 'M', 'G', 'T', 'P', 'E']
  let val = parseFloat(param || 0)
  let i = 0

  for (i = 0; (i < units.length) && (val >= base); i++) { val /= base }

  return val.toFixed(pr) + ' ' + units[i]
}

function formatTime (param) {
  let ts = param || 0
  let tm = 0
  let th = 0
  let td = 0

  if (ts > 59) {
    tm = Math.floor(ts / 60)
    ts = ts % 60
  }

  if (tm > 59) {
    th = Math.floor(tm / 60)
    tm = tm % 60
  }

  if (th > 23) {
    td = Math.floor(th / 24)
    th = th % 24
  }

  return td > 0 ? '%dd %dh %dm %ds'.format(td, th, tm, ts) : '%dh %dm %ds'.format(th, tm, ts)
}

// eslint-disable-next-line no-extend-native
String.prototype.format = function () {
  let str = this
  let out = ''
  const re = /^(([^%]*)%('.|0|\x20)?(-)?(\d+)?(\.\d+)?(%|b|c|d|u|f|o|s|x|X|t|m|M|))/
  let a = []
  let numSubstitutions = 0
  let subst = ''

  while ((a = re.exec(str)) !== null) {
    const m = a[1]
    const leftpart = a[2]; const pPad = a[3]; const pJustify = a[4]; const pMinLength = a[5]
    const pPrecision = a[6]; const pType = a[7]

    if (pType === '%') {
      subst = '%'
    } else {
      if (numSubstitutions < arguments.length) {
        const param = arguments[numSubstitutions++]

        let pad = ' '
        if (pPad && pPad.substr(0, 1) === '\'') { pad = leftpart.substr(1, 1) } else if (pPad) { pad = pPad }

        let justifyRight = true
        if (pJustify && pJustify === '-') { justifyRight = false }

        let minLength = -1
        if (pMinLength) { minLength = parseInt(pMinLength) }

        let precision = -1
        if (pPrecision && pType === 'f') { precision = parseInt(pPrecision.substring(1)) }

        subst = param

        switch (pType) {
          case 'b':
            subst = (parseInt(param) || 0).toString(2)
            break

          case 'c':
            subst = String.fromCharCode(parseInt(param) || 0)
            break

          case 'd':
            subst = (parseInt(param) || 0)
            break

          case 'u':
            subst = Math.abs(parseInt(param) || 0)
            break

          case 'f':
            subst = (precision > -1) ? ((parseFloat(param) || 0.0)).toFixed(precision) : (parseFloat(param) || 0.0)
            break

          case 'o':
            subst = (parseInt(param) || 0).toString(8)
            break

          case 's':
            subst = param
            break

          case 'x':
            subst = ('' + (parseInt(param) || 0).toString(16)).toLowerCase()
            break

          case 'X':
            subst = ('' + (parseInt(param) || 0).toString(16)).toUpperCase()
            break

          case 't':
            subst = formatTime(param)
            break

          case 'm':
            subst = formatUnit(param, 1000, pPrecision)
            break

          case 'M':
            subst = formatUnit(param, 1024, pPrecision)
            break
        }

        subst = (typeof (subst) === 'undefined') ? '' : subst.toString()

        if (minLength > 0 && pad.length > 0) {
          const padLength = minLength - subst.length
          for (let i = 0; i < padLength; i++) { subst = justifyRight ? (pad + subst) : (subst + pad) }
        }
      }
    }

    out += leftpart + subst
    str = str.substr(m.length)
  }

  return out + str
}
