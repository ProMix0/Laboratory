data <- read.table("./data.csv", header=TRUE, sep=";")

colnames(data)[5] <- 'стаж'
colnames(data)[6] <- 'процент.выпол.разраб.в_срок'
colnames(data)[7] <- 'колич.ошиб.от_пользов'
colnames(data)[8] <- 'бальн.оцен.заказчика'
colnames(data)[9] <- 'качеств.оцен.заказчика'
colnames(data)[10] <- 'кач.документир'

colnames(data)[1] <- 'index'
data$index <- NULL

data$группа <- as.factor(data$группа)
data$пол <- as.factor(data$пол)
data$качеств.оцен.заказчика <- as.factor(data$качеств.оцен.заказчика)

mode <- function(x) {
  ux <- unique(x)
  ux[which.max(tabulate(match(x, ux)))]
}

mystats <- function(x)
{
  if(is.numeric(x))
  {
    m <- mean(x)
  n <- length(x)
  s <- sd(x)
  skew <- sum((x-m)^3/s^3)/n
  kurt <- sum((x-m)^4/s^4)/n - 3
  return(c(mode=mode(x), min=min(x), quartile=quantile(x, c(0.25)), mean=m, median=median(x), quartile=quantile(x, c(0.75)), max=max(x), stdev=s, skew=skew, kurtosis=kurt)) 
  }else{
    return(c(mode=mode(x), min=NA, quartile=NA, mean=NA, median=NA, quartile=NA, max=NA, stdev=NA, skew=NA, kurtosis=NA)) 
    
 }
}

View(sapply(data, mystats))
View(sapply(subset(data,группа==1), mystats))
View(sapply(subset(data,группа==2), mystats))


par(mfrow=c(1,1))
plot(data.frame(data$процент.выпол.разраб.в_срок, data$стаж), main="Диаграмма рассеяния")


data1 <- data$пол
x <- c(summary(data1))
piepercent <- round(100*x/sum(x),1)
pie(x,piepercent,radius=1, main="Радиальная диаграмма по полу", col=c("lightgreen","lightpink"), clockwise = TRUE)
legend("topright", c("men", "women"), cex=0.8,fill=c("lightgreen", "lightpink"))

plot.new()
par (mfrow=c(2,2))
data1 <- data$качеств.оцен.заказчика
x1 <- c(summary(data1[data$пол==1&data$группа==1]))
x2 <- c(summary(data1[data$пол==1&data$группа==2]))
x3 <- c(summary(data1[data$пол==2&data$группа==1]))
x4 <- c(summary(data1[data$пол==2&data$группа==2]))
piepercent1 <- round(100*x1/sum(x1),1)
piepercent2 <- round(100*x2/sum(x2),1)
piepercent3 <- round(100*x3/sum(x3),1)
piepercent4 <- round(100*x4/sum(x4),1)
printpie <- function(data, labels, text){
  pie(data,labels,radius=1, main=text, col=c("#149","#773049", "pink"), clockwise = TRUE)
  legend("topright", c("Высокая", "Низкая", "Средняя"), cex=0.8,fill=c("#149","#773049", "pink"))
}
printpie(x1, piepercent1, "Оценка мужчин первой группы")
printpie(x2, piepercent2, "Оценка мужчин второй группы")
printpie(x3, piepercent3, "Оценка женщин первой группы")
printpie(x4, piepercent4, "Оценка женщин второй группы")

plot.new()
par(mfrow=c(1,1))
data_sorted <- with(data,data[order(стаж), ])
data_grouped <- split(data_sorted$возраст, list(data_sorted$пол, data_sorted$группа))
matrixq <- matrix(unlist(data_grouped), ncol=2)
barplot(matrixq,beside = TRUE, space =c (0,2.5), col=c("purple", "pink"), names.arg=unique(data_sorted$группа), main ="Диаграмма по стажу в зависимости пола и группы", xlab="Group", ylab="Age") 
legend("topright", legend=c("Мужчины", "Женщины"), fill=c("purple","pink"))

boxplot(возраст ~ пол, data = data, main = "Диаграмма размаха", xlab = "Пол", ylab = "Возраст", col = c("lightblue", "pink"))

plot.new()
num_numeric <- sum(unlist(lapply(data, is.numeric), use.names = FALSE))
par(mfrow=c(1,num_numeric))
for (i in seq_len(ncol(data))){
  if (is.numeric(data[,i]))
  {
    print(colnames(data)[i])
    hist(data[,i], xlab=colnames(data)[i], main=NA)
  }
}

plot.new()
par(mfrow = c(1, 1))
pairs(data[, c("возраст", "стаж",
               "процент.выпол.разраб.в_срок" ,
               "колич.ошиб.от_пользов", "бальн.оцен.заказчика", "кач.документир")],
      col = c("blue", "coral", "aquamarine", "purple", "lavender", "pink"),
      main = "Матричный график всех количественных признаков",
      cex = 0.5)

group1 <- subset(data,группа==1)
group2 <- subset(data,группа==2)

print(chisq.test(table(group1$пол,group1$качеств.оцен.заказчика)))
print(chisq.test(table(group2$пол,group2$качеств.оцен.заказчика)))

print(fisher.test(table(group1$пол,group1$качеств.оцен.заказчика)))
print(fisher.test(table(group2$пол,group2$качеств.оцен.заказчика)))

summary(aov(стаж ~ пол, data = data))

print(kruskal.test(стаж ~ пол, data = data))


