<template>
  <card-table title="Processes" :headers="headers" :items="items"></card-table>
</template>

<script>
  export default {
    data: () => ({
      headers: [
        {text: 'PID', value: 'pid'},
        {text: 'Owner', value: 'user'},
        {text: 'Command', value: 'command'},
        {text: 'CPU usage (%)', value: 'cpu_percent'},
        {text: 'Memory usage (%)', value: 'vsize_percent'},
      ],
      items: []
    }),
    mounted() {
      this.$ubus.call('vuci.system', 'process_list').then(r => {
        this.items = r.processes;
      });
    }
  };
</script>
