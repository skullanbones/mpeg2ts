var start_offset = 0;
var end_offset = 0;
var mpeg2ts_data = JSON.parse("{}");
chartColors = [
    'rgb(0, 255, 0)',
    'rgb(0, 0, 255)',
    'rgb(200, 200, 200)',
    'rgb(0, 255, 255)',
    'rgb(0, 128, 128)',
    'rgb(0, 255, 128)',
    'rgb(128, 255, 7)',
    'rgb(200, 255, 128)'
];
function fromUTF8Array(data)
{ // array of bytes
    var str = '', i;

    for (i = 0; i < data.length; i++)
    {
        var value = data[i];
        if (value < 0x80)
        {
            str += String.fromCharCode(value);
        } else if (value > 0xBF && value < 0xE0)
        {
            str += String.fromCharCode((value & 0x1F) << 6 | data[i + 1] & 0x3F);
            i += 1;
        } else if (value > 0xDF && value < 0xF0)
        {
            str += String.fromCharCode((value & 0x0F) << 12 | (data[i + 1] & 0x3F) << 6 | data[i + 2] & 0x3F);
            i += 2;
        } else
        {
            // surrogate pair
            var charCode = ((value & 0x07) << 18 | (data[i + 1] & 0x3F) << 12 | (data[i + 2] & 0x3F) << 6 | data[i + 3] & 0x3F) - 0x010000;

            str += String.fromCharCode(charCode >> 10 | 0xD800, charCode & 0x03FF | 0xDC00); 
            i += 3;
        }
    }

    return str;
}  
function parse_ts(file_content_as_typed_array)
{
    let tsData = Module._malloc(file_content_as_typed_array.length * file_content_as_typed_array.BYTES_PER_ELEMENT);
    Module.HEAPU8.set(file_content_as_typed_array, tsData); //copy from 1st arg to heap at offset tsData
    
    let jsonData = Module._malloc(1000000);
    
    let result = Module.ccall("webAsmEntryPoint", "number", ["number", "number", "number"], [tsData, file_content_as_typed_array.length, jsonData])
    var json_result_array = new Uint8Array(Module.HEAPU8.buffer, jsonData, result); // extract data to another JS array

    console.log("real result" + fromUTF8Array(json_result_array));
    json_loaded(fromUTF8Array(json_result_array))
};

function handleFileProgress(evt)
{
    console.log(evt.loaded)
}
function handleFileSelect(evt) 
{
    var files = evt.target.files; // FileList object

    // Loop through the FileList and render image files as thumbnails.
    for (var i = 0, f; f = files[i]; i++)
    {
        var reader = new FileReader();
        reader.addEventListener('progress', handleFileProgress);

        // Closure to capture the file information.
        reader.onload = (function(theFile)
        {
            return function(e)
            {
                let bufView = new Uint8Array(e.target.result);
                parse_ts(bufView);
            };
        })(f);

        reader.readAsArrayBuffer(f);
    }
}
document.getElementById('ts_file').addEventListener('change', handleFileSelect, false);
function new_start(newVal)
{
    start_offset = newVal;
    update_data();
}

function new_end(newVal)
{
    end_offset = newVal;
    update_data();
}

function json_loaded(json_as_string) 
{
    mpeg2ts_data = JSON.parse(json_as_string);
    let max_ofs = 0;
    config['data']['datasets'] = []
    for (let pid in mpeg2ts_data["stream"])
    {
        if (!pid.startsWith("Pid"))
        {
            continue;
        }
        if (mpeg2ts_data["stream"][pid][mpeg2ts_data["stream"][pid].length - 1]["ofs"] > max_ofs)
        {
            max_ofs = mpeg2ts_data["stream"][pid][mpeg2ts_data["stream"][pid].length - 1]["ofs"];
        }
        config['data']['datasets'].push({
                                            label: pid,
                                            pointBackgroundColor: [],
                                            borderColor : chartColors[config['data']['datasets'].length % 8],
                                            data: [],
                                            fill: false,
                                            pointRadius: 3,
                                        })
    }
    
    document.getElementById("slider_start").max = max_ofs;
    document.getElementById("slider_end").max = max_ofs;
    document.getElementById("slider_end").value = 1000000;
    end_offset = document.getElementById("slider_end").value;

    let ctx = document.getElementById('canvas').getContext('2d');
    window.myLine = new Chart(ctx, config);
    update_data();

    let ctx2 = document.getElementById('dts_histogram_canvas').getContext('2d');
    window.dts_histogram = new Chart(ctx2, dts_histogram_config);
    let ctx3 = document.getElementById('pts_histogram_canvas').getContext('2d');
    window.pts_histogram = new Chart(ctx3, pts_histogram_config);
    
    update_hist();
    update_stats();
}

function update_data() 
{
    let data_set = 0;
    for (let pid in mpeg2ts_data["stream"])
    {
        if (!pid.startsWith("Pid"))
        {
            continue;
        }
        config.data.datasets[data_set].data = [];
        config.data.datasets[data_set].pointBackgroundColor = [];
        for (let i = 0; i < mpeg2ts_data["stream"][pid].length; i++)
        {
            if (mpeg2ts_data["stream"][pid][i]["ofs"] < start_offset ||
                mpeg2ts_data["stream"][pid][i]["ofs"] > end_offset)
            {
                continue;
            }
            config.data.datasets[data_set].data.push({ 
                                                         x: mpeg2ts_data["stream"][pid][i]["ofs"],
                                                         y: parseInt(pid.substring(3))
                                                     });

            let atten = false;
            if (mpeg2ts_data["stream"][pid][i]["nals"])
            {
                for (let nal in mpeg2ts_data["stream"][pid][i]["nals"])
                {
                    if (mpeg2ts_data["stream"][pid][i]["nals"][nal]["type"] == 5 ||
                        mpeg2ts_data["stream"][pid][i]["nals"][nal]["type"] == 7 ||
                        mpeg2ts_data["stream"][pid][i]["nals"][nal]["type"] == 8)
                    {
                        atten = true;
                    }
                }
            
            }
            if (mpeg2ts_data["stream"][pid][i]["extra"])
            {
                atten = true;
            }
            if (mpeg2ts_data["stream"][pid][i]["pmtPids"] || mpeg2ts_data["stream"][pid][i]["streams"] || atten)
            {
                config.data.datasets[data_set].pointBackgroundColor.push('rgba(255, 0, 0, 1)');
            }
            else
            {
                config.data.datasets[data_set].pointBackgroundColor.push(chartColors[data_set % 8]);
            }
        }

        ++data_set;
    }
    window.myLine.update();
};
function update_hist() 
{
    dts_histogram_config['data']['datasets'] = [];
    pts_histogram_config['data']['datasets'] = [];
    for (let pid in mpeg2ts_data["stream"]["stats"])
    {
        if (!pid.startsWith("Pid"))
        {
            continue;
        }
        dts_histogram_config['data']['datasets'].push({
                                            label: pid,
                                            borderColor : chartColors[dts_histogram_config['data']['datasets'].length % 8],
                                            data: [],
                                            fill: false,
                                            pointRadius: 3,
                                          })
        pts_histogram_config['data']['datasets'].push({
                                            label: pid,
                                            borderColor : chartColors[pts_histogram_config['data']['datasets'].length % 8],
                                            data: [],
                                            fill: false,
                                            pointRadius: 3,
                                          })                                  
        let dtsHistogramData = dts_histogram_config['data']['datasets'][dts_histogram_config['data']['datasets'].length - 1]['data'];
        for (let i = 0; i < mpeg2ts_data["stream"]["stats"][pid]['DtsHist'].length; i++)
        {
            dtsHistogramData.push({ 
                                   x: mpeg2ts_data["stream"]["stats"][pid]['DtsHist'][i]["d"],
                                   y: parseInt(mpeg2ts_data["stream"]["stats"][pid]['DtsHist'][i]["n"])
                               });
        }
        let ptsHistogramData = pts_histogram_config['data']['datasets'][pts_histogram_config['data']['datasets'].length - 1]['data'];
        for (let i = 0; i < mpeg2ts_data["stream"]["stats"][pid]['PtsHist'].length; i++)
        {
            ptsHistogramData.push({ 
                                   x: mpeg2ts_data["stream"]["stats"][pid]['PtsHist'][i]["d"],
                                   y: parseInt(mpeg2ts_data["stream"]["stats"][pid]['PtsHist'][i]["n"])
                                  });
        }

    }
    window.dts_histogram.update();
    window.pts_histogram.update();
};

function update_stats() 
{
    let table = document.getElementById("pid_stats");
    for(var i = table.rows.length - 1; i > 0; i--)
    {
        table.deleteRow(i);
    }

    for (let pid in mpeg2ts_data["stream"]["stats"])
    {
        if (!pid.startsWith("Pid"))
        {
            continue;
        }
        let row = table.insertRow(table.rows.length);
        let cell1 = row.insertCell(0);
        cell1.innerHTML = pid.substring(3);
        let cell2 = row.insertCell(1);
        cell2.innerHTML = mpeg2ts_data["stream"]["stats"][pid]['discontinuity'];
        let cell3 = row.insertCell(2);
        cell3.innerHTML = mpeg2ts_data["stream"]["stats"][pid]['ccError'];
        let cell4 = row.insertCell(3);
        cell4.innerHTML = mpeg2ts_data["stream"]["stats"][pid]['Ptsmissing'];
        let cell5 = row.insertCell(4);
        cell5.innerHTML = mpeg2ts_data["stream"]["stats"][pid]['Dtsmissing'];
        
    }
};

var config = {
    type: 'scatter',
    data:
    {
        labels: [],
        datasets: [],
    },
    options:
    {
        responsive: true,
        title:
        {
            display: true,
        },
        tooltips:
        {
            mode: 'point',
            intersect: false,
            callbacks:
            {
                label: function(tooltipItem, data)
                {
                    let label = data.datasets[tooltipItem.datasetIndex].label || '';
                    if (label)
                    {
                        for (let item in mpeg2ts_data["stream"][label])
                        {
                            if (mpeg2ts_data["stream"][label][item]['ofs'] == tooltipItem.xLabel)
                            {
                                let pmt_pids = mpeg2ts_data["stream"][label][item]["pmtPids"];
                                let pmt_streams = mpeg2ts_data["stream"][label][item]["streams"];
                                let dts = mpeg2ts_data["stream"][label][item]["dts"];
                                let pts = mpeg2ts_data["stream"][label][item]["pts"];
                                let nals = mpeg2ts_data["stream"][label][item]["nals"];
                                let extra = mpeg2ts_data["stream"][label][item]["extra"];

                                if (pmt_pids)
                                {
                                    label = 'PAT with PMT pids ' + pmt_pids.toString();
                                }
                                if (pmt_streams)
                                {
                                    label = 'PMT with streams ';
                                    for (let str in pmt_streams)
                                    {
                                        label += '(' + pmt_streams[str]['pid'] + ', ' + pmt_streams[str]['type'] + ') '
                                    }
                                }
                                if (dts || pts)
                                {
                                    if (dts)
                                    {
                                        label += ', dts: ' + dts;
                                    }
                                    if (pts)
                                    {
                                        label += ', pts: ' + pts;
                                    }
                                }
                                if (nals)
                                {
                                    label += ', nal[';
                                    for (let nal in nals)
                                    {
                                        label +=  nals[nal]["type"] + ', ';
                                    }
                                    label += ']';
                                }
                                if (extra)
                                {
                                    label += extra;
                                }
                                break;
                            }
                        }
                    }
                    label += ' at ' + tooltipItem.xLabel;//Math.round(tooltipItem.yLabel * 100) / 100;
                    return label;
                }
            }
        },
        hover: {
            mode: 'point',
            intersect: false
        },
        scales: {
            xAxes: [{
                display: true,
                scaleLabel: {
                    display: true,
                    labelString: 'Byte offset'
                }
            }],
            yAxes: [{
                display: true,
                scaleLabel: {
                    display: true,
                    labelString: 'Pid'
                }
            }]
        }
    }
};
var dts_histogram_config = {
    type: 'scatter',
    data:
    {
        labels: [],
        datasets: [],
    },
    options:
    {
        responsive: true,
        title:
        {
            display: true,
        },
        tooltips:
        {
            mode: 'point',
            intersect: false
        },
        hover: {
            mode: 'point',
            intersect: false
        },
        scales: {
            xAxes: [{
                display: true,
                scaleLabel: {
                    display: true,
                    labelString: 'timestamp diff'
                }
            }],
            yAxes: [{
                display: true,
                scaleLabel: {
                    display: true,
                    labelString: 'DTSes quantity'
                }
            }]
        }
    }
};
var pts_histogram_config = {
    type: 'scatter',
    data:
    {
        labels: [],
        datasets: [],
    },
    options:
    {
        responsive: true,
        title:
        {
            display: true,
        },
        tooltips:
        {
            mode: 'point',
            intersect: false
        },
        hover: {
            mode: 'point',
            intersect: false
        },
        scales: {
            xAxes: [{
                display: true,
                scaleLabel: {
                    display: true,
                    labelString: 'timestamp diff'
                }
            }],
            yAxes: [{
                display: true,
                scaleLabel: {
                    display: true,
                    labelString: 'PTSes quantity'
                }
            }]
        }
    }
};