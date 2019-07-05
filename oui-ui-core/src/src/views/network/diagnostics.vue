<template>
  <div>
    <el-input v-model="host" style="width:240px"></el-input>
    <el-select v-model="tool" style="width:140px">
      <el-option v-for="tool in tools" :key="tool[0]" :value="tool[0]" :label="tool[1]"></el-option>
    </el-select>
    <el-button type="primary" @click="test">>></el-button>
    <el-input v-if="stdout !== ''" style="margin-top: 10px" type="textarea" autosize readonly :value="stdout"></el-input>
    <span v-if="stderr !== ''" style="color: red; display: block">{{ stderr }}</span>
  </div>
</template>

<script>
export default {
  data() {
    return {
      host: 'openwrt.org',
      tool: 'runPing',
      tools: [],
      stdout: '',
      stderr: ''
    }
  },
  methods: {
    runPing(name) {
      return this.$ubus.call('oui.network', 'ping', {name});
    },
    runPing6(name) {
      return this.$ubus.call('oui.network', 'ping6', {name});
    },
    runTraceroute(name) {
      return this.$ubus.call('oui.network', 'traceroute', {name});
    },
    runTraceroute6(name) {
      return this.$ubus.call('oui.network', 'traceroute6', {name});
    },
    runNslookup(name) {
      return this.$ubus.call('oui.network', 'nslookup', {name});
    },
    test() {
      const loading = this.$getLoading();

      this.stdout = '';
      this.stderr = '';

      this[this.tool](this.host).then(r => {
        if (r.stdout)
          this.stdout = r.stdout;

        if (r.stderr)
          this.stderr = r.stderr;
        loading.close();
      });
    }
  },
  created() {
    this.runPing('?').then(r => {
      if (r.code !== -1)
        this.tools.push(['runPing', 'IPv4 Ping']);
    });

    this.runPing6('?').then(r => {
      if (r.code !== -1)
        this.tools.push(['runPing6', 'IPv6 Ping']);
    });

    this.runTraceroute('?').then(r => {
      if (r.code !== -1)
        this.tools.push(['runTraceroute', 'IPv4 Traceroute']);
    });

    this.runTraceroute6('?').then(r => {
      if (r.code !== -1)
        this.tools.push(['runTraceroute6', 'IPv6 Traceroute']);
    });

    this.runNslookup('?').then(r => {
      if (r.code !== -1)
        this.tools.push(['runNslookup', 'DNS Lookup']);
    });
  }
}
</script>
