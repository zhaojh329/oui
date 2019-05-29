<template>
  <Table :columns="columns" :data="initscripts">
    <template slot-scope="{ row }" slot="EnableDisable">
      <Button :type="row.enabled ? 'info' : 'warning'" @click="EnableDisable(row)">{{ row.enabled ? 'Enabled' : 'Disabled' }}</Button>
    </template>
    <template slot-scope="{ row }" slot="action">
      <Button type="primary" style="margin-right: 5px" @click="action(row, 'start')">Start</Button>
      <Button type="primary" style="margin-right: 5px" @click="action(row, 'restart')">Restart</Button>
      <Button type="error" style="margin-right: 5px" @click="action(row, 'stop')">Stop</Button>
    </template>
  </Table>
</template>

<script>
export default {
  name: 'startup',
  data() {
    return {
      columns: [
        {key: 'start', title: 'Start priority', sortType: 'asc'},
        {key: 'name', title: 'Initscript'},
        {slot: 'EnableDisable', title: 'Enable/Disable'},
        {slot: 'action'}
      ],
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
        this.$system.initDisable(init.name).then(this.load);
      else
        this.$system.initEnable(init.name).then(this.load);
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