<template>
  <el-table :data="data">
    <el-table-column prop="pid" label="PID" width="100"></el-table-column>
    <el-table-column prop="user" label="Owner" width="100"></el-table-column>
    <el-table-column prop="command" label="Command"></el-table-column>
    <el-table-column prop="cpu_percent" label="CPU usage(%)" width="120"></el-table-column>
    <el-table-column prop="vsize_percent" label="Memory usage(%)" width="150"></el-table-column>
    <el-table-column width="300">
    <template v-slot="{ row }">
      <el-button type="primary" size="mini" @click="kill(row.pid, 1)">Hang Up</el-button>
      <el-button type="warning" size="mini" @click="kill(row.pid, 15)">Terminate</el-button>
      <el-button type="danger" size="mini" @click="kill(row.pid, 9)">Kill</el-button>
    </template>
    </el-table-column>
  </el-table>
</template>

<script>

export default {
  data() {
    return {
      data: []
    }
  },
  methods: {
    kill(pid, signum) {
      this.$ubus.call('system', 'signal', {pid, signum}).then(() => {
        this.$message.success(`Send signal ${signum} to ${pid} successfully.`);
      });
    }
  },
  created() {
    this.$ubus.call('oui.system', 'process_list').then(r => {
      this.data = r.processes;
    });
  }
}
</script>
