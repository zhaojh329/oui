<template>
  <el-table :data="initscripts" :default-sort="{prop: 'start', order: 'ascending'}">
    <el-table-column prop="start" :label="$t('Start priority')"></el-table-column>
    <el-table-column prop="name" :label="$t('Initscript')"></el-table-column>
    <el-table-column prop="name" label="#">
      <template v-slot="{ row }">
        <el-button :type="row.enabled ? 'success' : 'warning'" size="mini" @click="EnableDisable(row)">{{ row.enabled ? $t('Enabled') : $t('Disabled') }}</el-button>
      </template>
    </el-table-column>
    <el-table-column label="#">
      <template v-slot="{ row }">
        <el-button type="primary" size="mini" @click="action(row, 'start')">{{ $t('Start') }}</el-button>
        <el-button type="primary" size="mini" @click="action(row, 'restart')">{{ $t('Restart') }}</el-button>
        <el-button type="danger" size="mini" @click="action(row, 'stop')">{{ $t('Stop') }}</el-button>
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
  created() {
    this.load();
  }
}
</script>