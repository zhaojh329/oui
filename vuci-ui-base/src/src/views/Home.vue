<template>
  <v-app :dark="dark">
    <vuci-nav :drawer="drawer"></vuci-nav>
    <v-toolbar dark color="primary" dense fixed clipped-left app>
      <v-toolbar-side-icon @click.stop="drawer = !drawer"></v-toolbar-side-icon>
      <v-toolbar-title>{{hostname}}</v-toolbar-title>
      <v-spacer></v-spacer>
      <v-toolbar-items>
        <v-menu open-on-hover offset-y>
          <v-btn slot="activator" flat>Theme<v-icon>keyboard_arrow_down</v-icon></v-btn>
          <v-list>
            <v-list-tile @click="dark = true"><v-list-tile-title>Dark</v-list-tile-title></v-list-tile>
            <v-list-tile @click="dark = false"><v-list-tile-title>Light</v-list-tile-title></v-list-tile>
          </v-list>
        </v-menu>
        <v-menu open-on-hover offset-y>
          <v-btn slot="activator" flat>Language<v-icon>keyboard_arrow_down</v-icon></v-btn>
          <v-list>
            <v-list-tile><v-list-tile-title>中文</v-list-tile-title></v-list-tile>
            <v-list-tile><v-list-tile-title>English</v-list-tile-title></v-list-tile>
          </v-list>
        </v-menu>
        <v-menu open-on-hover offset-y>
          <v-btn slot="activator" flat>{{username}}<v-icon>keyboard_arrow_down</v-icon></v-btn>
          <v-list>
            <v-list-tile to="/login"><v-list-tile-title>Logout</v-list-tile-title></v-list-tile>
            <v-list-tile @click="$reboot"><v-list-tile-title>Reboot</v-list-tile-title></v-list-tile>
          </v-list>
        </v-menu>
      </v-toolbar-items>
    </v-toolbar>
    <v-content>
      <v-container fill-height>
        <v-layout column>
          <router-view></router-view>
        </v-layout>
      </v-container>
      <v-footer height="auto" color="primary" app>
        <v-layout justify-center>
          &copy;2018 — <strong>Jianhui zhao</strong>
        </v-layout>
      </v-footer>
    </v-content>
  </v-app>
</template>

<script>
  import VuciNav from '../components/VuciNav.vue'

  export default {
    components: {
      'vuci-nav': VuciNav
    },
    data: () => ({
      dark: false,
      drawer: true,
      hostname: '',
      username: sessionStorage.getItem('username'),
      dialog: false,
      rebooting: false
    }),
    created() {
      this.$system.getInfo().then(r => {
        this.hostname = r.hostname;
      });
    }
  };
</script>
