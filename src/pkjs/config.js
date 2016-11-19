module.exports = [
  {
    "type": "heading",
    "defaultValue": "Legend of Zelda Watch Settings"
  },
  {
    "type": "text",
    "defaultValue": "Please allow 3-5 seconds for the watch face to update after saving."
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Settings"
      },
      {
        "type": "select",
        "messageKey": "BG_COLOR",
        "defaultValue": "green",
        "label": "Background Color",
        "options": [
          {
            "label": "Green",
            "value": "green"
          },
          {
            "label": "Red",
            "value": "red"
          }
        ]
      },
      {
        "type": "radiogroup",
        "messageKey": "DATE_FORMAT",
        "label": "Date Format",
        "defaultValue": "usa",
        "options": [
          {
            "label": "MM/DD/YY",
            "value": "usa"
          },
          {
            "label": "DD/MM/YY",
            "value": "global"
          }
        ]
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];