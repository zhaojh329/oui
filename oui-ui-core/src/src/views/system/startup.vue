<template>
  <el-table :data="initscripts" :default-sort="{prop: 'start', order: 'ascending'}">
    <el-table-column prop="start" label="Start priority"></el-table-column>
    <el-table-column prop="name" label="Initscript"></el-table-column>
    <el-table-column prop="name" label="Enable/Disable">
      <template v-slot="{ row }">
        <el-button :type="row.enabled ? 'success' : 'warning'" size="mini" @click="EnableDisable(row)">{{ row.enabled ? 'Enabled' : 'Disabled' }}</el-button>
      </template>
    </el-table-column>
    <el-table-column>
      <template v-slot="{ row }">
        <el-button type="primary" size="mini" @click="action(row, 'start')">Start</el-button>
        <el-button type="primary" size="mini" @click="action(row, 'restart')">Restart</el-button>
        <el-button type="danger" size="mini" @click="action(row, 'stop')">Stop</el-button>
      </template>
    </el-table-column>
  </el-table>
</template>

<script>
import { setTimeout } from 'timers';
export default {
  name: 'startup',
  data() {
    return {
      initscripts: []
    }
  },
  methods: {
    load() {
      this.$system.initList().then(initscripts => {
        this.initscripts = initscripts.filter(item => typeof(item.start) !== 'undefined');
      });
    },
    EnableDisable(init) {
      if (init.enabled)
        this.$system.initDisable(init.name);
      else
        this.$system.initEnable(init.name);

      setTimeout(() => {
        this.load();
      }, 1000);
    },
    action(init, op) {
      if (op === 'start')
        this.$system.initStart(init.name).then(this.load);
      else if (op === 'restart')
        this.$system.initRestart(init.name).then(this.load);
      else
        this.$system.initStop(init.name).then(this.load);
    }
  },
  mounted() {
    this.load();
  }
}
</script>