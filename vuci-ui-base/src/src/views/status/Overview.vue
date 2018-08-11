<template>
  <v-layout>
    <v-layout column fill-height>
      <v-card class="item">
        <v-card-title>
          <span>System</span>
        </v-card-title>
        <v-card-text>
          <v-text-field label="Kernel" value="4.14.54"></v-text-field>
          <v-text-field label="System" value="MediaTek MT7620A ver:2 eco:6"></v-text-field>
          <v-text-field label="Model" value="Xiaomi MiWiFi Mini"></v-text-field>
          <v-text-field label="Target" value="ramips/mt7620"></v-text-field>
          <v-text-field label="Revision" value="r7621-063f8d236c"></v-text-field>
        </v-card-text>
      </v-card>
    </v-layout>
    <v-layout column fill-height>
      <v-card  class="item">
        <v-card-text>
           <v-list two-line>
            <v-list-tile avatar>
              <v-list-tile-avatar>
                <v-icon color="primary" x-large>access_time</v-icon>
              </v-list-tile-avatar>
              <v-list-tile-content>
                <v-list-tile-title>{{uptime | format('t')}}</v-list-tile-title>
                <v-list-tile-title>Uptime</v-list-tile-title>
              </v-list-tile-content>
            </v-list-tile>
           </v-list>
        </v-card-text>
      </v-card>
      <v-card  class="item">
        <v-card-text>
           <v-list three-line>
            <v-list-tile avatar>
              <v-list-tile-avatar>
                <v-icon color="primary" x-large>memory</v-icon>
              </v-list-tile-avatar>
              <v-list-tile-content>
                <v-list-tile-title>12/60 MB</v-list-tile-title>
                <v-list-tile-title>
                  <v-progress-linear v-model="valueDeterminate"></v-progress-linear>
                </v-list-tile-title>
                <v-list-tile-title>Memory</v-list-tile-title>
              </v-list-tile-content>
            </v-list-tile>
           </v-list>
        </v-card-text>
      </v-card>
      <v-card  class="item">
        <v-card-text>
           <v-list three-line>
            <v-list-tile avatar>
              <v-list-tile-avatar>
                <v-icon color="primary" x-large>battery_std</v-icon>
              </v-list-tile-avatar>
              <v-list-tile-content>
                <v-list-tile-title>{{load}}%</v-list-tile-title>
                <v-list-tile-title>
                  <v-progress-linear v-model="valueDeterminate"></v-progress-linear>
                </v-list-tile-title>
                <v-list-tile-title>Load</v-list-tile-title>
              </v-list-tile-content>
            </v-list-tile>
           </v-list>
        </v-card-text>
      </v-card>
      <v-card  class="item">
        <v-card-text>
           <v-list three-line>
            <v-list-tile avatar>
              <v-list-tile-avatar>
                <v-icon color="primary" x-large>sd_storage</v-icon>
              </v-list-tile-avatar>
              <v-list-tile-content>
                <v-list-tile-title>12/60 MB</v-list-tile-title>
                <v-list-tile-title>
                  <v-progress-linear v-model="valueDeterminate"></v-progress-linear>
                </v-list-tile-title>
                <v-list-tile-title>Free Disk</v-list-tile-title>
              </v-list-tile-content>
            </v-list-tile>
           </v-list>
        </v-card-text>
      </v-card>
    </v-layout>
  </v-layout>
</template>

<script>
  export default {
    data: () => ({
      valueDeterminate: 50,
      uptime: 12212,
      memory: 12123123,
      load: 12.12,
      storage: 20.2
    })
  };
</script>

<style>
.item {
  margin: 10px;
}
</style>