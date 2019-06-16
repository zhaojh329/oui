function esc(s, r) {
  for (let i = 0; i < r.length; i += 2)
    s = s.replace(r[i], r[i + 1]);
  return s;
}

function formatUnit(param, pMinLength, pPrecision) {
  let mf = pMinLength ? parseInt(pMinLength) : 1000;
  let pr = pPrecision ? Math.floor(10 * parseFloat('0' + pPrecision)) : 2;

  let i = 0;
  let val = parseFloat(param || 0);
  let units = ['', 'K', 'M', 'G', 'T', 'P', 'E'];

  for (i = 0; (i < units.length) && (val > mf); i++)
    val /= mf;

  return val.toFixed(pr) + ' ' + units[i];
}

function formatTime(param) {
  let ts = param || 0;
  let tm = 0;
  let th = 0;
  let td = 0;

  if (ts > 59) {
    tm = Math.floor(ts / 60);
    ts = ts % 60;
  }

  if (tm > 59) {
    th = Math.floor(tm / 60);
    tm = tm % 60;
  }

  if (th > 23) {
    td = Math.floor(th / 24);
    th = th % 24;
  }

  return td > 0 ? '%dd %dh %dm %ds'.format(td, th, tm, ts) : '%dh %dm %ds'.format(th, tm, ts);
}

String.prototype.format = function() {
  const html_esc = [/&/g, '&#38;', /"/g, '&#34;', /'/g, '&#39;', /</g, '&#60;', />/g, '&#62;'];
  const quot_esc = [/"/g, '&#34;', /'/g, '&#39;'];

  let str = this;
  let out = '';
  let re = /^(([^%]*)%('.|0|\x20)?(-)?(\d+)?(\.\d+)?(%|b|c|d|u|f|o|s|x|X|q|h|j|t|m))/;
  let a = [];
  let numSubstitutions = 0;

  while ((a = re.exec(str)) !== null) {
    let m = a[1];
    let leftpart = a[2], pPad = a[3], pJustify = a[4], pMinLength = a[5];
    let pPrecision = a[6], pType = a[7], subst;

    if (pType === '%') {
      subst = '%';
    } else {
      if (numSubstitutions < arguments.length) {
        let param = arguments[numSubstitutions++];

        let pad = ' ';
        if (pPad && pPad.substr(0, 1) === '\'')
          pad = leftpart.substr(1, 1);
        else if (pPad)
          pad = pPad;

        let justifyRight = true;
        if (pJustify && pJustify === '-')
          justifyRight = false;

        let minLength = -1;
        if (pMinLength)
          minLength = parseInt(pMinLength);

        let precision = -1;
        if (pPrecision && pType === 'f')
          precision = parseInt(pPrecision.substring(1));

        subst = param;

        switch (pType) {
        case 'b':
          subst = (parseInt(param) || 0).toString(2);
          break;

        case 'c':
          subst = String.fromCharCode(parseInt(param) || 0);
          break;

        case 'd':
          subst = (parseInt(param) || 0);
          break;

        case 'u':
          subst = Math.abs(parseInt(param) || 0);
          break;

        case 'f':
          subst = (precision > -1) ? ((parseFloat(param) || 0.0)).toFixed(precision) : (parseFloat(param) || 0.0);
          break;

        case 'o':
          subst = (parseInt(param) || 0).toString(8);
          break;

        case 's':
          subst = param;
          break;

        case 'x':
          subst = ('' + (parseInt(param) || 0).toString(16)).toLowerCase();
          break;

        case 'X':
          subst = ('' + (parseInt(param) || 0).toString(16)).toUpperCase();
          break;

        case 'h':
          subst = esc(param, html_esc);
          break;

        case 'q':
          subst = esc(param, quot_esc);
          break;

        case 'j':
          subst = String.serialize(param);
          break;

        case 't':
          subst = formatTime(param);
          break;

        case 'm':
          subst = formatUnit(param, pMinLength, pPrecision);
          break;
        }

        subst = (typeof(subst) === 'undefined') ? '' : subst.toString();

        if (minLength > 0 && pad.length > 0) {
          let padLength = minLength - subst.length;
          for (let i = 0; i < padLength; i++)
            subst = justifyRight ? (pad + subst) : (subst + pad);
        }
      }
    }

    out += leftpart + subst;
    str = str.substr(m.length);
  }

  return out + str;
}
