<template>
  <Table :columns="columns" :data="data" :height="tableHeight">
    <template slot-scope="{ row }" slot="action">
      <Button type="primary" style="margin-right: 5px" @click="kill(row.pid, 1)">Hang Up</Button>
      <Button type="warning" style="margin-right: 5px" @click="kill(row.pid, 15)">Terminate</Button>
      <Button type="error" style="margin-right: 5px" @click="kill(row.pid, 9)">Kill</Button>
    </template>
  </Table>
</template>

<script>

export default {
  name: 'processes',
  data() {
    return {
      tableHeight: 0,
      columns: [
        {key: 'pid', title: 'PID', sortType: 'asc', width: 100},
        {key: 'user', title: 'Owner', width: 100},
        {key: 'command', title: 'Command'},
        {key: 'cpu_percent', title: 'CPU usage(%)', width: 120},
        {key: 'vsize_percent', title: 'Memory usage(%)', width: 150},
        {slot: 'action', width: 300}
      ],
      data: []
    }
  },
  methods: {
    kill(pid, signum) {
      this.$ubus.call('system', 'signal', {pid, signum});
    },
    updateTableHeight() {
      this.tableHeight = document.body.clientHeight - 100;
    }
  },
  mounted() {
    this.updateTableHeight();
    window.addEventListener('resize', () => {
      this.updateTableHeight();
    });

    this.$ubus.call('oui.system', 'process_list').then(r => {
      this.data = r.processes;
    });
  }
}
</script>
