<template>
  <div>
    <a-input v-model="host" style="width:240px; margin-right: 10px"/>
    <a-select v-model="tool" style="width:160px">
      <a-select-option v-for="tool in tools" :key="tool[0]" :value="tool[0]">{{ tool[1] }}</a-select-option>
    </a-select>
    <a-button type="primary" @click="test" :loading="loading">>></a-button>
    <a-textarea v-if="stdout !== ''" style="margin-top: 10px" auto-size readonly :value="stdout"/>
    <span v-if="stderr !== ''" style="color: red; display: block">{{ stderr }}</span>
  </div>
</template>

<script>
export default {
  data () {
    return {
      host: 'openwrt.org',
      tool: 'runPing',
      tools: [],
      stdout: '',
      stderr: '',
      loading: false
    }
  },
  methods: {
    runPing (name) {
      return this.$ubus.call('oui.network', 'ping', { name })
    },
    runPing6 (name) {
      return this.$ubus.call('oui.network', 'ping6', { name })
    },
    runTraceroute (name) {
      return this.$ubus.call('oui.network', 'traceroute', { name })
    },
    runTraceroute6 (name) {
      return this.$ubus.call('oui.network', 'traceroute6', { name })
    },
    runNslookup (name) {
      return this.$ubus.call('oui.network', 'nslookup', { name })
    },
    test () {
      this.loading = true

      this.stdout = ''
      this.stderr = ''

      this[this.tool](this.host).then(r => {
        if (r.stdout) { this.stdout = r.stdout }

        if (r.stderr) { this.stderr = r.stderr }
        this.loading = false
      })
    }
  },
  created () {
    this.runPing('?').then(r => {
      if (r.code !== -1) { this.tools.push(['runPing', 'IPv4 Ping']) }
    })

    this.runPing6('?').then(r => {
      if (r.code !== -1) { this.tools.push(['runPing6', 'IPv6 Ping']) }
    })

    this.runTraceroute('?').then(r => {
      if (r.code !== -1) { this.tools.push(['runTraceroute', 'IPv4 Traceroute']) }
    })

    this.runTraceroute6('?').then(r => {
      if (r.code !== -1) { this.tools.push(['runTraceroute6', 'IPv6 Traceroute']) }
    })

    this.runNslookup('?').then(r => {
      if (r.code !== -1) { this.tools.push(['runNslookup', 'DNS Lookup']) }
    })
  }
}
</script>
