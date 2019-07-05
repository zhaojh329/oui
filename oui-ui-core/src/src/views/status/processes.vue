<template>
  <el-table :data="data" :default-sort="{prop: 'pid', order: 'ascending'}">
    <el-table-column prop="pid" label="PID" width="100"></el-table-column>
    <el-table-column prop="user" :label="$t('Owner')" width="100"></el-table-column>
    <el-table-column prop="command" :label="$t('Command')" show-overflow-tooltip></el-table-column>
    <el-table-column prop="cpu_percent" :label="$t('CPU usage')" width="100" :formatter="row => row.cpu_percent + '%'"></el-table-column>
    <el-table-column prop="vsize_percent" :label="$t('Memory usage')" width="100" :formatter="row => row.vsize_percent + '%'"></el-table-column>
    <el-table-column width="320" :label="$t('Signal')">
      <template v-slot="{ row }">
        <el-button type="primary" size="mini" @click="kill(row.pid, 1)">{{ $t('Hang Up') }}</el-button>
        <el-button type="warning" size="mini" @click="kill(row.pid, 15)">{{ $t('Terminate') }}</el-button>
        <el-button type="danger" size="mini" @click="kill(row.pid, 9)">{{ $t('Kill immediately') }}</el-button>
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
