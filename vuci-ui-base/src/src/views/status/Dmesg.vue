<template>
  <card-table title="Kernel Log" :headers="headers" :items="log" hide-actions hide-headers></card-table>
</template>

<script>
  export default {
    data() {
      return {
        headers: [{value: 'msg'}],
        log: []
      }
    },
    mounted() {
      this.$ubus.call('vuci.system', 'dmesg').then(r => {
        let lines = r.log.replace(/\n+$/, '').split(/\n/);
        this.log = lines.map(line => {
          return {msg: line}
        });
      });
    }
  };
</script>
