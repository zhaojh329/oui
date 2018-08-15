<template>
  <v-card>
    <v-card-title primary-title>
      Running Processes
      <v-spacer></v-spacer>
      <v-text-field v-model="search" append-icon="search" label="Search" single-line hide-details></v-text-field>
    </v-card-title>
    <v-divider></v-divider>
    <v-data-table :headers="headers" :items="items" :search="search">
      <template slot="items" slot-scope="props">
        <td>{{props.item.pid}}</td>
        <td>{{props.item.user}}</td>
        <td>{{props.item.cpu_percent}}</td>
        <td>{{props.item.vsize_percent}}</td>
        <td>{{props.item.command}}</td>
        <td>
          <v-menu offset-y left>
            <v-btn slot="activator" flat icon><v-icon>more_vert</v-icon></v-btn>
            <v-list>
              <v-list-tile @click="signal(props.item.pid, 1)">
                <v-list-tile-action><v-icon>redo</v-icon></v-list-tile-action>
                <v-list-tile-title>Reload - HUP</v-list-tile-title>
              </v-list-tile>
              <v-list-tile @click="signal(props.item.pid, 15)">
                <v-list-tile-action><v-icon>clear</v-icon></v-list-tile-action>
                <v-list-tile-title>Terminate - INT</v-list-tile-title></v-list-tile>
              <v-list-tile @click="signal(props.item.pid, 9)">
                <v-list-tile-action><v-icon>flash_on</v-icon></v-list-tile-action>
                <v-list-tile-title>Kill - KILL</v-list-tile-title>
              </v-list-tile>
            </v-list>
          </v-menu>
        </td>
      </template>
    </v-data-table>
  </v-card>
</template>

<script>
  export default {
    data() {
      return {
        search: '',
        headers: [
          {text: 'PID', value: 'pid', width: '10'},
          {text: 'Owner', value: 'user', width: '10'},
          {text: 'CPU usage (%)', value: 'cpu_percent', width: '10'},
          {text: 'Memory usage (%)', value: 'vsize_percent', width: '10'},
          {text: 'Command', value: 'command'},
          {text: 'Signal', sortable: false}
        ],
        items: []
      }
    },
    mounted() {
      this.load();
    },
    methods: {
      load() {
        this.$ubus.call('vuci.system', 'process_list').then(r => {
          this.items = r.processes;
        });
      },
      signal(pid, signum) {
        this.$ubus.call('system', 'signal', {pid, signum}).then(() => {
          this.load();
        });
      }
    }
  };
</script>
