<template>
  <v-navigation-drawer v-model="drawer" fixed clipped app>
    <v-list>
      <v-list-group no-action v-for="m in menus" :key="m.title">
        <v-list-tile slot="activator">
          <v-list-tile-content>
            <v-list-tile-title>{{m.title}}</v-list-tile-title>
          </v-list-tile-content>
        </v-list-tile>
        <v-list-tile v-for="sm in m.childs" :key="sm.title" :to="sm.path">
          <v-list-tile-content>
            <v-list-tile-title>{{sm.title}}</v-list-tile-title>
          </v-list-tile-content>
        </v-list-tile>
      </v-list-group>
    </v-list>
  </v-navigation-drawer>
</template>

<script>
  export default {
    name: 'vuci-nav',
    props: {
      drawer: {
        type: Boolean,
        default: true
      }
    },
    data: () => ({
      menus: []
    }),
    methods: {
      toChildTree(menu) {
        const root = {title: 'root', index: 0, childs: {}};
        let node;

        if (!menu) return root;

        for (const key in menu) {
          if (menu.hasOwnProperty(key)) {
            const path = key.split(/\//);
            node = root;

            for (let i = 0; i < path.length; i++) {
              if (!node.childs)
                node.childs = {};

              if (!node.childs[path[i]])
                node.childs[path[i]] = {path: '/' + path.slice(0, i + 1).join('/')};
              node = node.childs[path[i]];
            }

            Object.assign(node, menu[key]);
            if (!node.title)
              node.title = node.path;
          }
        }

        return root;
      },
      toChildArray(node) {
        const childs = [];

        if (!node.childs)
          return node;

        for (const key in node.childs) {
          if (node.childs.hasOwnProperty(key)) {
            this.toChildArray(node.childs[key]);
            childs.push(node.childs[key]);
          }
        }

        childs.sort((a, b) => (a.index || 0) - (b.index || 0));

        if (childs.length) {
          node.childs = childs;
        } else {
          delete node.childs;
        }

        return node;
      }
    },
    created() {
      this.$ubus.call('vuci.ui', 'menu').then(r => {
        this.menus = this.toChildArray(this.toChildTree(r.menu)).childs;
      });
    }
  };
</script>
