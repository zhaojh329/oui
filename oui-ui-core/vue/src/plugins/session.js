import { rpc } from './rpc'

export const session = {}

session.sid = function () {
  return sessionStorage.getItem('sid') || ''
}

session.username = function () {
  return sessionStorage.getItem('username') || ''
}

session.login = function (username, password) {
  return new Promise(resolve => {
    if (typeof (password) !== 'string') {
      password = ''
    }

    rpc.login(username, password).then(r => {
      sessionStorage.setItem('sid', r.sid)
      sessionStorage.setItem('username', username)
      this.startHeartbeat()
      resolve(true)
    }).catch(() => {
      resolve(false)
    })
  })
}

session.logout = function () {
  return new Promise(resolve => {
    const sid = this.sid()
    if (!sid) {
      resolve()
      return
    }

    rpc.logout().then(() => {
      resolve()
    }).catch(() => {
      resolve()
    })

    this.stopHeartbeat()
    sessionStorage.removeItem('sid')
    sessionStorage.removeItem('username')
  })
}

session.isAlive = function () {
  return new Promise(resolve => {
    rpc.alive().then(() => {
      resolve(true)
    }).catch(() => {
      resolve(false)
    })
  })
}

session.startHeartbeat = function () {
  if (typeof (this._hearbeatInterval) !== 'undefined') {
    return
  }

  this._hearbeatInterval = window.setInterval(() => {
    this.isAlive().then(alive => {
      if (!alive) {
        this.logout()
      }
    })
  }, 15000)
}

session.stopHeartbeat = function () {
  if (typeof (this._hearbeatInterval) !== 'undefined') {
    window.clearInterval(this._hearbeatInterval)
    delete this._hearbeatInterval
  }
}

export default {
  install (Vue) {
    Vue.prototype.$session = session
  }
}
